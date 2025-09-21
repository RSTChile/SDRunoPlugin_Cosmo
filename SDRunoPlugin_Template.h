#pragma once
#include "iunoplugin.h"
#include "iunoaudioprocessor.h"
#include <memory>
#include <atomic>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>

class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoAudioProcessor {
public:
    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    // IUnoPlugin override
    void HandleEvent(const UnoEvent& ev) override;

    // IUnoAudioProcessor override (IQOUT como floats intercalados)
    void AudioProcessorProcess(channel_t channel, float* buffer, int length, bool& modified) override;

    void LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg);

    float CalculateRC(const std::vector<float>& iq);
    float CalculateINR(const std::vector<float>& iq);
    float CalculateLF(float rc, float inr);
    float CalculateRDE(float rc, float inr);

    std::string DetectPalimpsesto(const std::vector<float>& iq);

    void SetModeRestrictivo(bool restrictivo);
    bool GetModeRestrictivo() const;

    void UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);

    // Señales desde GUI (asíncronas)
    void RequestUnloadAsync();
    void SetCaptureEnabled(bool enabled);                  // Botón Capturar
    void RequestChangeBaseDirAsync(const std::string& p);  // Cambio de carpeta
    void ChangeVrxAsync(int newIndex);                     // Futuro: re-registrar VRX (actualmente fijo en 0)

    // Leídos por la GUI (thread-safe)
    std::string GetBaseDirSafe() const;

private:
    void EnsureUiStarted();
    void UpdateReference(const std::vector<float>& iq);

    // Gestión de archivos IQ
    enum class Mode { Restrictivo = 0, Funcional = 1 };
    void ApplyPendingModeIfAny();
    void ApplyPendingBaseDirIfAny();
    void ApplyPendingVrxIfAny();
    void RotateIqFile(Mode mode);
    void CloseIqFile();
    void AppendIq(const std::vector<float>& iq);
    std::string BuildBaseDataDir();
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
    std::atomic<bool> m_unloadRequested{false};
    std::atomic<bool> m_isUnloading{false};
    std::atomic<bool> m_closingDown{false};

    // Telemetría
    std::chrono::steady_clock::time_point m_lastTick{};

    // Streaming / Modo
    std::atomic<bool> m_isStreaming{false};
    std::atomic<bool> m_modeChangeRequested{false};
    std::atomic<int>  m_pendingMode{0}; // 0=Restrictivo, 1=Funcional
    Mode m_activeMode{Mode::Restrictivo};

    // Captura
    std::atomic<bool> m_captureEnabled{false};

    // Archivos IQ
    std::ofstream m_iqOut;
    std::string   m_currentFilePath;

    // Carpeta base (concurrencia protegida)
    mutable std::mutex m_configMutex;
    std::string   m_baseDir;
    std::string   m_pendingBaseDir;
    std::atomic<bool> m_baseDirChangeRequested{false};

    // VRX
    int m_vrxIndex{0}; // por ahora usamos siempre VRX 0 como en los ejemplos
    std::atomic<bool> m_vrxChangeRequested{false};
    std::atomic<int>  m_pendingVrxIndex{0};
};
