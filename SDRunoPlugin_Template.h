#pragma once

// Orden estable de la API (como en plugins de referencia)
#include "iunoplugin.h"
#include "iunoplugincontroller.h"
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

    // IUnoPlugin
    void HandleEvent(const UnoEvent& ev) override;

    // IUnoAudioProcessor (IQOUT a 192 kS/s; floats intercalados con I/Q invertidos por SDRuno)
    void AudioProcessorProcess(channel_t channel, float* buffer, int length, bool& modified) override;

    // Señales desde la UI
    void SetModeRestrictivo(bool restrictivo);
    bool GetModeRestrictivo() const;
    void SetCaptureEnabled(bool enabled);
    void RequestChangeBaseDirAsync(const std::string& p);
    void ChangeVrxAsync(int newIndex);
    void RequestUnloadAsync();
    std::string GetBaseDirSafe() const;

    // Métricas/helpers
    void LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg);
    float CalculateRC(const std::vector<float>& iq);
    float CalculateINR(const std::vector<float>& iq);
    float CalculateLF(float rc, float inr);
    float CalculateRDE(float rc, float inr);
    std::string DetectPalimpsesto(const std::vector<float>& iq);
    void UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);

private:
    void EnsureUiStarted();
    void UpdateReference(const std::vector<float>& iq);

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

    // Carpeta base (protegida)
    mutable std::mutex m_configMutex;
    std::string   m_baseDir;
    std::string   m_pendingBaseDir;
    std::atomic<bool> m_baseDirChangeRequested{false};

    // VRX (VRX 0 por defecto; se puede cambiar desde Settings)
    int m_vrxIndex{0};
    std::atomic<bool> m_vrxChangeRequested{false};
    std::atomic<int>  m_pendingVrxIndex{0};
};
