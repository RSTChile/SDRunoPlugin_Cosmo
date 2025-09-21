#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>
#include <unoevent.h>
#include <filesystem>
#include <ctime>

#ifdef _WIN32
// Evita que Windows.h defina macros min/max que rompen std::min/std::max
#define NOMINMAX
#include <Windows.h>
#include <ShlObj.h>
#include <KnownFolders.h>
#endif

using std::string;

static string WideToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int size = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string out(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), out.data(), size, nullptr, nullptr);
    return out;
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
        iq.reserve(static_cast<size
