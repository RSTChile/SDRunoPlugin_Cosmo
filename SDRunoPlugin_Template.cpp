#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <string>
#include <vector>
#include <filesystem>
#include <ctime>
#include <unoevent.h>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <KnownFolders.h>
#endif

using std::string;

static string WideToUtf8(const std::wstring& w) {
#ifdef _WIN32
    if (w.empty()) return {};
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
    : IUnoPlugin(controller), m_controller(controller)
{
    logFile.open("cosmo_metrics_log.csv", std::ios::out);
    if (logFile.is_open())
        logFile << "RC,INR,LF,RDE,MSG\n";

    m_lastTick = std::chrono::steady_clock::now();
    m_baseDir = BuildBaseDataDir();

    m_vrxIndex = 0;
    EnsureUiStarted();

    // Si el plugin se carga con el streaming ya activo, vincularse de inmediato
    int n = 1;
    try { n = m_controller.GetVRXCount(); } catch (...) { n = 1; }
    m_registered.assign((size_t)n, false);

    bool anyOn = false;
    for (int ch = 0; ch < n; ++ch) {
        bool en = false, st = false;
        try { en = m_controller.GetVRXEnable((channel_t)ch); } catch (...) {}
        try { st = m_controller.IsStreamingEnabled((channel_t)ch); } catch (...) {}
        if (en && st) { anyOn = true; break; }
    }
    if (anyOn) {
        BindAllStreamProcessors();
        m_isStreaming.store(true, std::memory_order_release);
        if (m_ui) m_ui->SetStreamingState(true);
    }
}

SDRunoPlugin_Template::~SDRunoPlugin_Template() {
    try { UnbindAllStreamProcessors(); } catch (...) {}
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

    UnbindAllStreamProcessors();
    CloseIqFile();
    try { m_controller.RequestUnload(this); } catch (...) {}
}

// Procesamiento del stream baseband (I/Q). No toca el demodulador ni el audio.
void SDRunoPlugin_Template::StreamProcessorProcess(channel_t channel, Complex* buffer, int length, bool& modified) {
    modified = false;
    if (buffer == nullptr || length <= 0) return;

    try {
        EnsureUiStarted();

        if ((int)channel != m_vrxIndex) {
            m_vrxIndex = (int)channel;
        }

        if (!m_isStreaming.exchange(true)) {
            if (m_ui) m_ui->SetStreamingState(true);
        }

        ApplyPendingBaseDirIfAny();
        ApplyPendingModeIfAny();
        ApplyPendingVrxIfAny();

        // Complex -> [I,Q] float32
        std::vector<float> iq;
        iq.resize((size_t)length * 2);
        for (int i = 0; i < length; ++i) {
            iq[2 * i + 0] = buffer[i].real;
            iq[2 * i + 1] = buffer[i].imag;
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
        EnsureUiStarted();

        switch (ev.GetType()) {
        case UnoEvent::StreamingStarted:
            BindAllStreamProcessors();
            m_isStreaming.store(true, std::memory_order_release);
            if (m_ui) m_ui->SetStreamingState(true);
            break;

        case UnoEvent::StreamingStopped:
            m_isStreaming.store(false, std::memory_order_release);
            if (m_ui) m_ui->SetStreamingState(false);
            UnbindAllStreamProcessors();
            CloseIqFile();
            if (m_ui) { m_ui->UpdateSavePath(""); }
            break;

        case UnoEvent::ClosingDown:
            m_closingDown.store(true, std::memory_order_release);
            UnbindAllStreamProcessors();
            CloseIqFile();
            break;

        default:
            break;
        }

        ApplyPendingVrxIfAny();
        ProcessUnloadIfRequested();

        if (m_ui) { m_ui->HandleEvent(ev); }
    } catch (const std::exception& ex) {
        if (logFile.is_open()) { logFile << "0,0,0,0,\"HandleEvent EXCEPTION: " << ex.what() << "\"\n"; logFile.flush(); }
    } catch (...) {
        if (logFile.is_open()) { logFile << "0,0,0,0,\"HandleEvent EXCEPTION: unknown\"\n"; logFile.flush(); }
    }
}

// ====== Métricas y heurísticas ======

float SDRunoPlugin_Template::CalculateRC(const std::vector<float>& iq) {
    size_t N = iq.size() / 2;
    if (N == 0) return 0.0f;
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

std::string SDRunoPlugin_Template::DetectPalimpsesto(const std::vector<float>& iq) {
    size_t N = iq.size() / 2;
    int nPrimos = 0;
    int nPicosEnPrimos = 0;

    for (size_t i = 2; i < N; ++i) {
        bool primo = true;
        for (size_t d = 2; d * d <= i; ++d) {
            if (i % d == 0) { primo = false; break; }
        }
        if (primo) {
            ++nPrimos;
            float I = iq[2 * i];
            float Q = iq[2 * i + 1];
            float mag = std::sqrt(I * I + Q * Q);
            if (mag > 2.0f) ++nPicosEnPrimos;
        }
    }

    if (nPrimos > 0 && static_cast<float>(nPicosEnPrimos) / nPrimos > 0.2f) {
        return "Patrón palimpsesto detectado en índices primos.";
    }
    return "";
}

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

// ====== Interacción con UI ======

void SDRunoPlugin_Template::SetModeRestrictivo(bool restrictivo) {
    modoRestrictivo = restrictivo;
    m_pendingMode.store(restrictivo ? 0 : 1, std::memory_order_release);
    m_modeChangeRequested.store(true, std::memory_order_release);
}

bool SDRunoPlugin_Template::GetModeRestrictivo() const { return modoRestrictivo; }

void SDRunoPlugin_Template::SetCaptureEnabled(bool enabled) {
    bool was = m_captureEnabled.exchange(enabled, std::memory_order_acq_rel);
    if (enabled == was) return;

    if (!enabled) {
        CloseIqFile();
        if (m_ui) m_ui->UpdateSavePath("");
        return;
    }

    // Forzar rotación acorde al modo activo la próxima vez que escribamos
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

std::string SDRunoPlugin_Template::GetBaseDirSafe() const {
    std::lock_guard<std::mutex> lk(m_configMutex);
    return m_baseDir;
}

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

    m_vrxIndex = next;
}

// ====== Archivos ======

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
    if (!m_iqOut.is_open() || iq.empty()) return;
    const char* data = reinterpret_cast<const char*>(iq.data());
    size_t bytes = iq.size() * sizeof(float);
    try { m_iqOut.write(data, (std::streamsize)bytes); }
    catch (...) { try { CloseIqFile(); } catch (...) {} }
}

// ====== Registro de stream: todos los VRX habilitados ======

void SDRunoPlugin_Template::BindAllStreamProcessors() {
    int n = 1;
    try { n = m_controller.GetVRXCount(); } catch (...) { n = 1; }
    if ((int)m_registered.size() != n) m_registered.assign((size_t)n, false);

    for (int ch = 0; ch < n; ++ch) {
        bool enabled = true;
        bool streaming = true;
        try { enabled = m_controller.GetVRXEnable((channel_t)ch); } catch (...) {}
        try { streaming = m_controller.IsStreamingEnabled((channel_t)ch); } catch (...) {}
        if (!enabled || !streaming) { m_registered[ch] = false; continue; }

        if (m_registered[ch]) continue;
        try {
            m_controller.RegisterStreamProcessor((channel_t)ch, this);
            m_registered[ch] = true;
        } catch (...) {
            m_registered[ch] = false;
        }
    }
}

void SDRunoPlugin_Template::UnbindAllStreamProcessors() {
    if (m_registered.empty()) return;
    for (int ch = 0; ch < (int)m_registered.size(); ++ch) {
        if (!m_registered[ch]) continue;
        try {
            m_controller.UnregisterStreamProcessor((channel_t)ch, this);
        } catch (...) {}
        m_registered[ch] = false;
    }
}
