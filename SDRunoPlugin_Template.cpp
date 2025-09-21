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
// Evita que Windows.h defina macros min/max que rompen std::min/std::max
#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <KnownFolders.h>
#endif

using std::string;

// Conversión segura a UTF-8 evitando choque con identificadores "size"
static string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
#ifdef _WIN32
    // Calcular cantidad de bytes necesarios
    int needed = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return {};
    std::string out;
    out.resize(static_cast<size_t>(needed));
    // Convertir (sin terminador NUL extra)
    int written = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), static_cast<int>(w.size()), out.data(), needed, nullptr, nullptr);
    if (written <= 0) return {};
    return out;
#else
    return {};
#endif
}

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller)
    : IUnoPlugin(controller)
{
    // Registrar observador IQ en VRX 0 por defecto
    m_vrxIndex = 0;
    controller.RegisterStreamObserver(m_vrxIndex, this);

    // Logging
    logFile.open("cosmo_metrics_log.csv", std::ios::out);
    logFile << "RC,INR,LF,RDE,MSG\n";
    m_lastTick = std::chrono::steady_clock::now();

    // Base dir para datos
    m_baseDir = BuildBaseDataDir();
}

SDRunoPlugin_Template::~SDRunoPlugin_Template() {
    // Detener callbacks primero (protegido)
    try { m_controller.UnregisterStreamObserver(m_vrxIndex, this); } catch (...) {}

    // Cerrar archivo IQ
    try { CloseIqFile(); } catch (...) {}

    // Destruir UI después de detener callbacks
    m_ui.reset();

    if (logFile.is_open()) logFile.close();
}

void SDRunoPlugin_Template::EnsureUiStarted() {
    if (!m_uiStarted.exchange(true)) {
        m_ui = std::make_unique<SDRunoPlugin_TemplateUi>(*this, m_controller);
    }
}

void SDRunoPlugin_Template::RequestUnloadAsync() {
    // No solicitar unload si el host está cerrando
    if (m_closingDown.load(std::memory_order_acquire)) return;

    // Garantizar una sola solicitud
    bool expected = false;
    if (!m_isUnloading.compare_exchange_strong(expected, true)) return;
    m_unloadRequested.store(true, std::memory_order_release);
}

void SDRunoPlugin_Template::StreamObserverProcess(channel_t /*channel*/, const Complex* buffer, int length) {
    try {
        // Ejecutar petición pendiente de unload en hilo del plugin/host
        if (m_unloadRequested.exchange(false, std::memory_order_acq_rel)) {
            try { m_controller.RequestUnload(this); } catch (...) {}
            return;
        }

        // Iniciar UI perezosamente
        EnsureUiStarted();

        // Si recibimos muestras, marcamos streaming activo (independiente del evento)
        if (!m_isStreaming.exchange(true)) {
            if (m_ui) m_ui->SetStreamingState(true);
        }

        // Aplicar cambios pendientes (VRX y carpeta) antes de procesar
        ApplyPendingVrxIfAny();
        ApplyPendingBaseDirIfAny();

        // Aplicar cambio de modo pendiente (si viene del hilo GUI)
        ApplyPendingModeIfAny();

        // Preparar IQ actual
        std::vector<float> iq;
        iq.reserve(static_cast<size_t>(length) * 2);
        for (int i = 0; i < length; ++i) {
            iq.push_back(static_cast<float>(buffer[i].real));
            iq.push_back(static_cast<float>(buffer[i].imag));
        }

        if (!haveRef) { UpdateReference(iq); haveRef = true; }

        float rc = CalculateRC(iq);
        float inr = CalculateINR(iq);
        float lf = CalculateLF(rc, inr);
        float rde = CalculateRDE(rc, inr);

        std::string palimpsestoMsg = DetectPalimpsesto(iq);

        std::string msg;
        if (!modoRestrictivo) {
            if (lf > 0.5f && rde > 0.3f) {
                msg = "¿Y si hay un patrón oculto? NO SÉ. DISIENTO.";
            } else if (lf < 0.05f) {
                msg = "NO SÉ: señal estéril.";
            } else if (!palimpsestoMsg.empty()) {
                msg = palimpsestoMsg;
            }
        } else {
            if (!palimpsestoMsg.empty()) msg = palimpsestoMsg;
        }

        UpdateUI(rc, inr, lf, rde, msg, modoRestrictivo);
        LogMetrics(rc, inr, lf, rde, msg);

        // Escribir a disco sólo si "Capturar" está activado
        if (m_captureEnabled.load(std::memory_order_acquire)) {
            if (!m_iqOut.is_open()) {
                RotateIqFile(m_activeMode);
            }
            AppendIq(iq);
        }

        // Tick cada ~1s para confirmar procesamiento
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
    size_t N = (std::min)(iq.size(), refSignal.size()); // evitar macro min de Windows
    if (N == 0) return 1.0f;
    float err = 0.0f, refMag = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float e = iq[i] - refSignal[i];
        err += e * e;
        refMag += refSignal[i] * refSignal[i];
    }
    if (refMag == 0.0f) return 1.0f;
    float rmse = std::sqrt(err / static_cast<float>(N));
    float norm = std::sqrt(refMag / static_cast<float>(N));
    return (std::min)(1.0f, rmse / (norm + 1e-6f)); // evitar macro min de Windows
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
    if (m_ui) {
        m_ui->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
    }
}

// Manejo de eventos endurecido: no dejar escapar excepciones al host
void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev) {
    try {
        switch (ev.GetType()) {
        case UnoEvent::StreamingStarted:
            m_isStreaming.store(true, std::memory_order_release);
            if (m_ui) m_ui->SetStreamingState(true);
            break;
        case UnoEvent::StreamingStopped:
            m_isStreaming.store(false, std::memory_order_release);
            if (m_ui) m_ui->SetStreamingState(false);
            CloseIqFile();
            if (m_ui) { m_ui->UpdateSavePath(""); }
            break;
        case UnoEvent::ClosingDown:
            m_closingDown.store(true, std::memory_order_release);
            break;
        default:
            break;
        }

        if (m_ui) { m_ui->HandleEvent(ev); }
    } catch (const std::exception& ex) {
        if (logFile.is_open()) {
            logFile << "0,0,0,0,\"HandleEvent EXCEPTION: " << ex.what() << "\"\n"; logFile.flush();
        }
    } catch (...) {
        if (logFile.is_open()) {
            logFile << "0,0,0,0,\"HandleEvent EXCEPTION: unknown\"\n"; logFile.flush();
        }
    }
}

std::string SDRunoPlugin_Template::DetectPalimpsesto(const std::vector<float>& iq) {
    size_t N = iq.size() / 2;
    int nPrimos = 0, nPicosEnPrimos = 0;
    for (size_t i = 2; i < N; ++i) {
        bool primo = true;
        for (size_t d = 2; d * d <= i; ++d) if (i % d == 0) { primo = false; break; }
        if (primo) {
            nPrimos++;
            float I = iq[2 * i];
            float Q = iq[2 * i + 1];
            float mag = std::sqrt(I * I + Q * Q);
            if (mag > 2.0f) nPicosEnPrimos++;
        }
    }
    if (nPrimos > 0 && (float)nPicosEnPrimos / nPrimos > 0.2f)
        return "Patrón palimpsesto detectado en índices primos.";
    return "";
}

void SDRunoPlugin_Template::SetModeRestrictivo(bool restrictivo) {
    modoRestrictivo = restrictivo;
    m_pendingMode.store(restrictivo ? 0 : 1, std::memory_order_release);
    m_modeChangeRequested.store(true, std::memory_order_release);
}

bool SDRunoPlugin_Template::GetModeRestrictivo() const { return modoRestrictivo; }

// ===== Señales desde GUI =====
void SDRunoPlugin_Template::SetCaptureEnabled(bool enabled) {
    m_captureEnabled.store(enabled, std::memory_order_release);
    if (!enabled) {
        CloseIqFile();
        if (m_ui) m_ui->UpdateSavePath("");
    } else {
        // Forzar apertura en siguiente bloque
        m_modeChangeRequested.store(true, std::memory_order_release);
        m_pendingMode.store(modoRestrictivo ? 0 : 1, std::memory_order_release);
    }
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
    try {
        m_controller.UnregisterStreamObserver(m_vrxIndex, this);
    } catch (...) {}
    try {
        m_controller.RegisterStreamObserver(next, this);
        m_vrxIndex = next;
    } catch (...) {
        // Si falla, intenta restaurar el anterior
        try { m_controller.RegisterStreamObserver(m_vrxIndex, this); } catch (...) {}
    }
}

// ===== Utilidades de archivo =====
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
    // Usar ProgramData\CosmoSDRuno\examples como carpeta base
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
        if (m_ui) {
            // Informar nueva ruta al UI (muestra "Captura de Señal: …" si streaming)
            try { m_ui->UpdateSavePath(m_currentFilePath); } catch (...) {}
        }
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
    try {
        m_iqOut.write(data, static_cast<std::streamsize>(bytes));
    } catch (...) {
        try { CloseIqFile(); } catch (...) {}
    }
}
