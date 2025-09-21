#pragma once
#include "iunoplugin.h"
#include "iunostreamobserver.h"
#include <memory>
#include <atomic>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoStreamObserver {
public:
    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    // IUnoPlugin override
    void HandleEvent(const UnoEvent& ev) override;

    // IUnoStreamObserver override
    void StreamObserverProcess(channel_t channel, const Complex* buffer, int length) override;

    void LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg);

    float CalculateRC(const std::vector<float>& iq);
    float CalculateINR(const std::vector<float>& iq);
    float CalculateLF(float rc, float inr);
    float CalculateRDE(float rc, float inr);

    std::string DetectPalimpsesto(const std::vector<float>& iq);

    void SetModeRestrictivo(bool restrictivo);
    bool GetModeRestrictivo() const;

    void UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);

    // Señal desde hilo GUI: solicitar unload de forma segura (se ejecuta en hilo del plugin)
    void RequestUnloadAsync();

private:
    void EnsureUiStarted();
    void UpdateReference(const std::vector<float>& iq);

    // Gestión de archivos IQ
    enum class Mode { Restrictivo = 0, Funcional = 1 };
    void ApplyPendingModeIfAny();  // aplicar cambio de modo en hilo del plugin
    void RotateIqFile(Mode mode);  // cerrar/abrir archivo para el modo
    void CloseIqFile();
    void AppendIq(const std::vector<float>& iq);
    std::string BuildBaseDataDir();               // Documentos\\SDRuno\\Cosmo
    static std::string BuildTimestamp();
    static std::string ModeToString(Mode m);

private:
    std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;
    std::atomic<bool> m_uiStarted{false};
    std::ofstream logFile;

    // Métricas
    std::vector<float> refSignal;
    bool haveRef{false};
    bool modoRestrictivo{true};

    // Coordinación de unload
    std::atomic<bool> m_unloadRequested{false}; // consumido en hilo del plugin
    std::atomic<bool> m_isUnloading{false};     // una sola vez por ciclo de vida
    std::atomic<bool> m_closingDown{false};     // se marca en evento ClosingDown

    // Telemetría
    std::chrono::steady_clock::time_point m_lastTick{};

    // Streaming / Modo
    std::atomic<bool> m_isStreaming{false};
    std::atomic<bool> m_modeChangeRequested{false};
    std::atomic<int>  m_pendingMode{0}; // 0=Restrictivo, 1=Funcional
    Mode m_activeMode{Mode::Restrictivo};

    // Archivos IQ
    std::ofstream m_iqOut;
    std::string   m_currentFilePath;
    std::string   m_baseDir;
};
