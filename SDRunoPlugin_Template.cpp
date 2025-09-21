#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>
#include <unoevent.h>
#include <filesystem>
#include <ctime>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <KnownFolders.h>
#endif

using std::string;

static string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
#ifdef _WIN32
    int needed = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return {};
    std::string out; out.resize((size_t)needed);
    int written = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), out.data(), needed, nullptr, nullptr);
    if (written <= 0) return {};
    return out;
#else
    return {};
#endif
}

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller)
    : IUnoPlugin(controller)
{
    logFile.open("cosmo_metrics_log.csv", std::ios::out);
    logFile << "RC,INR,LF,RDE,MSG\n";
    m_lastTick = std::chrono::steady_clock::now();
    m_baseDir = BuildBaseDataDir();

    // No tocar demod al iniciar; capturaremos solo cuando Capturar=ON
    m_vrxIndex = 0;
}

SDRunoPlugin_Template::~SDRunoPlugin_Template() {
    try { UnbindAudioProcessor(); } catch (...) {}
    try { RestoreDemodIfNeeded(); } catch (...) {}
    try { CloseIqFile(); } catch (...) {}
    m_ui.reset();
    if (logFile.is_open()) logFile.close();
}

void SDRunoPlugin_Template::EnsureUiStarted() {
    if (!m_uiStarted.exchange(true)) {
        m_ui = std::make_unique<SDRunoPlugin_TemplateUi>(*this, m_controller);
    }
}

void SDRunoPlugin_Template::RequestUnloadAsync() {
    if (m_closingDown.load(std::memory_order_acquire)) return;
    bool expected = false;
    if (!m_isUnloading.compare_exchange_strong(expected, true)) return;
    m_unloadRequested.store(true, std::memory_order_release);
}

void SDRunoPlugin_Template::ProcessUnloadIfRequested() {
    if (!m_isUnloading.load(std::memory_order_acquire)) return;
    bool doUnload = m_unloadRequested.exchange(false, std::memory_order_acq_rel);
    if (!doUnload) return;
    // Limpiar antes de pedir unload
    UnbindAudioProcessor();
    RestoreDemodIfNeeded();
    CloseIqFile();
    try { m_controller.RequestUnload(this); } catch (...) {}
}

void SDRunoPlugin_Template::AudioProcessorProcess(channel_t channel, float* buffer, int length, bool& modified) {
    // Nunca tocar el audio de salida
    modified = false;

    // Solo procesar si estamos realmente en IQOUT (captura activa)
    if (!m_inIqOut.load(std::memory_order_acquire))
        return;

    try {
        EnsureUiStarted();

        if ((int)channel != m_vrxIndex) {
            m_vrxIndex = (int)channel;
        }

        if (!m_isStreaming.exchange(true)) {
            if (m_ui) m_ui->SetStreamingState(true);
        }

        // Aplicar cambios pendientes
        ApplyPendingBaseDirIfAny();
        ApplyPendingModeIfAny();
        ApplyPendingVrxIfAny();

        // SDRuno IQOUT: buffer de floats intercalados y con I/Q invertidos:
        // I = buffer[2*i + 1], Q = buffer[2*i + 0]
        std::vector<float> iq;
        iq.reserve((size_t)length * 2);
        for (int i = 0; i < length; ++i) {
            float I = buffer[2 * i + 1];
            float Q = buffer[2 * i + 0];
            iq.push_back(I);
            iq.push_back(Q);
        }

        if (!haveRef) { UpdateReference(iq); haveRef = true; }

        float rc  = CalculateRC(iq);
        float inr = CalculateINR(iq);
        float lf  = CalculateLF(rc, inr);
        float rde = CalculateRDE(rc, inr);

        std::string pal = DetectPalimpsesto(iq);
        std::string msg;
        if (!modoRestrictivo) {
            if (lf > 0.5f && rde > 0.3f)      msg = "¿Y si hay un patrón oculto? NO SÉ. DISIENTO.";
            else if (lf < 0.05f)              msg = "NO SÉ: señal estéril.";
            else if (!pal.empty())            msg = pal;
        } else if (!pal.empty())              msg = pal;

        UpdateUI(rc, inr, lf, rde, msg, modoRestrictivo);
        LogMetrics(rc, inr, lf, rde, msg);

        if (m_captureEnabled.load(std::memory_order_acquire)) {
            if (!m_iqOut.is_open()) {
                RotateIqFile(m_activeMode);
            }
            AppendIq(iq);
        }

        auto now = std::chrono::steady_clock::now();
        if (now - m_lastTick > std::chrono::seconds(1)) {
            if (logFile.is_open()) { logFile << "tick,," << lf << "," << rde << ",\"processing\"\n"; logFile.flush(); }
            m_lastTick = now;
        }
    }
    catch (const std::exception& ex) {
        if (logFile.is_open()) { logFile << "0,0,0,0,\"EXCEPTION: " << ex.what() << "\"\n"; logFile.flush(); }
    }
    catch (...) {
        if (logFile.is_open()) { logFile << "0,0,0,0,\"EXCEPTION: unknown\"\n"; logFile.flush(); }
    }
}

void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev) {
    try {
        switch (ev.GetType()) {
        case UnoEvent::StreamingStarted:
            if (m_captureEnabled.load(std::memory_order_acquire)) {
                EngageIqOutOnVrx(m_vrxIndex);
                RebindAudioProcessor(m_vrxIndex);
            }
            m_isStreaming.store(true, std::memory_order_release);
            if (m_ui) m_ui->SetStreamingState(true);
            break;

        case UnoEvent::StreamingStopped:
            m_isStreaming.store(false, std::memory_order_release);
            if (m_ui) m_ui->SetStreamingState(false);
            UnbindAudioProcessor();
            RestoreDemodIfNeeded();
            CloseIqFile();
            if (m_ui) { m_ui->UpdateSavePath(""); }
            break;

        case UnoEvent::ClosingDown:
            m_closingDown.store(true, std::memory_order_release);
            break;

        default:
            break;
        }

        // Aplicar cambios pendientes (incluye VRX)
        ApplyPendingVrxIfAny();

        // Descargar de forma segura si fue solicitado
        ProcessUnloadIfRequested();

        if (m_ui) { m_ui->HandleEvent(ev); }
    } catch (const std::exception& ex) {
        if (logFile.is_open()) { logFile << "0,0,0,0,\"HandleEvent EXCEPTION: " << ex.what() << "\"\n"; logFile.flush(); }
    } catch (...) {
        if (logFile.is_open()) { logFile << "0,0,0,0,\"HandleEvent EXCEPTION: unknown\"\n"; logFile.flush(); }
    }
}

float SDRunoPlugin_Template::CalculateRC(const std::vector<float>& iq) {
    size_t N = iq.size() / 2;
    float total = 0.0f, band = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float I = iq[2 * i];
        float Q = iq[2 * i + 1];
        float mag = std::sqrt(I * I + Q * Q);
        total += mag;
        if (i > N * 3 / 4) band += mag;
    }
    if (total == 0.0f) return 0.0f;
    return band / total;
}

float SDRunoPlugin_Template::CalculateINR(const std::vector<float>& iq) {
    size_t N = (std::min)(iq.size(), refSignal.size());
    if (N == 0) return 1.0f;
    float err = 0.0f, refMag = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float e = iq[i] - refSignal[i];
        err += e * e;
        refMag += refSignal[i] * refSignal[i];
    }
    if (refMag == 0.0f) return 1.0f;
    float rmse = std::sqrt(err / (float)N);
    float norm = std::sqrt(refMag / (float)N);
    return (std::min)(1.0f, rmse / (norm + 1e-6f));
}

float SDRunoPlugin_Template::CalculateLF(float rc, float inr) { return rc * (1.0f - inr); }
float SDRunoPlugin_Template::CalculateRDE(float rc, float inr) { return rc * inr; }

void SDRunoPlugin_Template::LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg) {
    if (logFile.is_open()) {
        logFile << rc << "," << inr << "," << lf << "," << rde << ",\"" << msg << "\"\n";
        logFile.flush();
    }
}

void SDRunoPlugin_Template::UpdateReference(const std::vector<float>& iq) { refSignal = iq; }

void SDRunoPlugin_Template::UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo) {
    if (m_ui) { m_ui->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo); }
}

void SDRunoPlugin_Template::SetModeRestrictivo(bool restrictivo) {
    modoRestrictivo = restrictivo;
    m_pendingMode.store(restrictivo ? 0 : 1, std::memory_order_release);
    m_modeChangeRequested.store(true, std::memory_order_release);
}

bool SDRunoPlugin_Template::GetModeRestrictivo() const { return modoRestrictivo; }

// ===== Señales desde GUI =====
void SDRunoPlugin_Template::SetCaptureEnabled(bool enabled) {
    bool was = m_captureEnabled.exchange(enabled, std::memory_order_acq_rel);

    if (enabled == was) return;

    if (!enabled) {
        // Apagando captura: desregistrar y restaurar demod
        UnbindAudioProcessor();
        RestoreDemodIfNeeded();
        CloseIqFile();
        if (m_ui) m_ui->UpdateSavePath("");
        m_inIqOut.store(false, std::memory_order_release);
        return;
    }

    // Encendiendo captura: poner VRX actual en IQOUT y registrar procesador
    EngageIqOutOnVrx(m_vrxIndex);
    RebindAudioProcessor(m_vrxIndex);

    // Forzar rotación de archivo acorde al modo activo
    m_modeChangeRequested.store(true, std::memory_order_release);
    m_pendingMode.store(modoRestrictivo ? 0 : 1, std::memory_order_release);
}

void SDRunoPlugin_Template::RequestChangeBaseDirAsync(const std::string& p) {
    std::lock_guard<std::mutex> lk(m_configMutex);
    m_pendingBaseDir = p;
    m_baseDirChangeRequested.store(true, std::memory_order_release);
}

void SDRunoPlugin_Template::ChangeVrxAsync(int newIndex) {
    m_pendingVrxIndex.store(newIndex, std::memory_order_release);
    m_vrxChangeRequested.store(true, std::memory_order_release);
}

// ===== Lecturas thread-safe para GUI =====
std::string SDRunoPlugin_Template::GetBaseDirSafe() const {
    std::lock_guard<std::mutex> lk(m_configMutex);
    return m_baseDir;
}

// ===== Aplicaciones de cambios pendientes =====
void SDRunoPlugin_Template::ApplyPendingModeIfAny() {
    if (!m_modeChangeRequested.exchange(false, std::memory_order_acq_rel))
        return;
    Mode newMode = (m_pendingMode.load(std::memory_order_acquire) == 0) ? Mode::Restrictivo : Mode::Funcional;
    if (newMode != m_activeMode || !m_iqOut.is_open()) {
        RotateIqFile(newMode);
        m_activeMode = newMode;
    }
}

void SDRunoPlugin_Template::ApplyPendingBaseDirIfAny() {
    if (!m_baseDirChangeRequested.exchange(false, std::memory_order_acq_rel))
        return;
    {
        std::lock_guard<std::mutex> lk(m_configMutex);
        if (!m_pendingBaseDir.empty())
            m_baseDir = m_pendingBaseDir;
    }
    CloseIqFile();
    if (m_captureEnabled.load(std::memory_order_acquire)) {
        RotateIqFile(m_activeMode);
    }
}

void SDRunoPlugin_Template::ApplyPendingVrxIfAny() {
    if (!m_vrxChangeRequested.exchange(false, std::memory_order_acq_rel))
        return;

    int next = m_pendingVrxIndex.load(std::memory_order_acquire);
    if (next == m_vrxIndex) return;

    if (m_captureEnabled.load(std::memory_order_acquire)) {
        // Restaurar el VRX anterior si estaba en IQOUT por nosotros
        RestoreDemodIfNeeded();
        UnbindAudioProcessor();

        // Activar IQOUT en el nuevo y registrar
        m_vrxIndex = next;
        EngageIqOutOnVrx(m_vrxIndex);
        RebindAudioProcessor(m_vrxIndex);
    } else {
        // Solo cambiar el índice activo
        m_vrxIndex = next;
    }
}

// ===== Utilidades =====
std::string SDRunoPlugin_Template::BuildTimestamp() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    tm = *std::localtime(&t);
#endif
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%04d%02d%02d_%02d%02d%02d",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

std::string SDRunoPlugin_Template::ModeToString(Mode m) {
    return (m == Mode::Restrictivo) ? "restrictivo" : "funcional";
}

std::string SDRunoPlugin_Template::BuildBaseDataDir() {
#ifdef _WIN32
    PWSTR pathW = nullptr;
    string base;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr, &pathW))) {
        base = WideToUtf8(pathW);
        CoTaskMemFree(pathW);
    } else {
        base = "C:\\ProgramData";
    }
    std::filesystem::path p = std::filesystem::path(base) / "CosmoSDRuno" / "examples";
    std::error_code ec;
    std::filesystem::create_directories(p, ec);
    return p.string();
#else
    std::filesystem::path p = std::filesystem::path("CosmoData");
    std::error_code ec;
    std::filesystem::create_directories(p, ec);
    return p.string();
#endif
}

void SDRunoPlugin_Template::RotateIqFile(Mode mode) {
    try {
        CloseIqFile();
        const auto ts = BuildTimestamp();
        std::filesystem::path base;
        {
            std::lock_guard<std::mutex> lk(m_configMutex);
            base = m_baseDir;
        }
        std::filesystem::path fname = base / ("cosmo_" + ts + "_" + ModeToString(mode) + ".iqf");
        m_iqOut.open(fname, std::ios::binary | std::ios::out);
        m_currentFilePath = fname.string();
        if (m_ui) { try { m_ui->UpdateSavePath(m_currentFilePath); } catch (...) {} }
    } catch (...) {
        if (logFile.is_open()) logFile << "0,0,0,0,\"RotateIqFile failed\"\n";
    }
}

void SDRunoPlugin_Template::CloseIqFile() {
    if (m_iqOut.is_open()) {
        try { m_iqOut.flush(); } catch (...) {}
        try { m_iqOut.close(); } catch (...) {}
    }
}

void SDRunoPlugin_Template::AppendIq(const std::vector<float>& iq) {
    if (!m_iqOut.is_open()) return;
    const char* data = reinterpret_cast<const char*>(iq.data());
    size_t bytes = iq.size() * sizeof(float);
    try { m_iqOut.write(data, (std::streamsize)bytes); }
    catch (...) { try { CloseIqFile(); } catch (...) {} }
}

// ===== Gestión IQOUT =====
void SDRunoPlugin_Template::EngageIqOutOnVrx(int vrx) {
    try {
        // Guardar demod original solo si no la habíamos guardado o si cambiamos de VRX
        IUnoPluginController::DemodulatorType current = m_controller.GetDemodulatorType(vrx);
        m_savedDemodType = current;
        m_savedDemodVrx = vrx;
        m_savedDemodValid = true;

        m_controller.SetVRXEnable(vrx, true);
        m_controller.SetDemodulatorType(vrx, IUnoPluginController::DemodulatorIQOUT);
        m_inIqOut.store(true, std::memory_order_release);
    } catch (...) {
        // Si falla, dejamos marcado como no IQOUT para evitar procesar audio
        m_inIqOut.store(false, std::memory_order_release);
    }
}

void SDRunoPlugin_Template::RestoreDemodIfNeeded() {
    if (!m_savedDemodValid) return;
    try {
        m_controller.SetDemodulatorType(m_savedDemodVrx, m_savedDemodType);
    } catch (...) {}
    m_savedDemodValid = false;
    m_inIqOut.store(false, std::memory_order_release);
}

void SDRunoPlugin_Template::RebindAudioProcessor(int vrx) {
    if (m_audioProcRegistered.load(std::memory_order_acquire)) {
        try { m_controller.UnregisterAudioProcessor(vrx, this); } catch (...) {}
        m_audioProcRegistered.store(false, std::memory_order_release);
    }
    try {
        m_controller.RegisterAudioProcessor(vrx, this);
        m_audioProcRegistered.store(true, std::memory_order_release);
    } catch (...) {
        m_audioProcRegistered.store(false, std::memory_order_release);
    }
}

void SDRunoPlugin_Template::UnbindAudioProcessor() {
    if (!m_audioProcRegistered.load(std::memory_order_acquire)) return;
    try { m_controller.UnregisterAudioProcessor(m_vrxIndex, this); } catch (...) {}
    m_audioProcRegistered.store(false, std::memory_order_release);
}
