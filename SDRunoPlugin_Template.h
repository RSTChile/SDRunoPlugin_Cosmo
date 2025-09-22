#pragma once

#include "iunoplugin.h"
#include "iunostreamprocessor.h"
#include "complex.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class SDRunoPlugin_TemplateUi;
class IUnoPluginController;

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoStreamProcessor
{
public:
    enum class Mode { Restrictivo = 0, Funcional = 1 };

    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    // IUnoPlugin
    virtual void HandleEvent(const UnoEvent& ev) override;

    // IUnoStreamProcessor (reemplaza el uso de IUnoAudioProcessor)
    virtual void StreamProcessorProcess(channel_t channel, Complex* buffer, int length, bool& modified) override;

    // UI hooks
    void SetModeRestrictivo(bool restrictivo);
    bool GetModeRestrictivo() const;

    void SetCaptureEnabled(bool enabled);
    void RequestChangeBaseDirAsync(const std::string& p);
    void ChangeVrxAsync(int newIndex);
    std::string GetBaseDirSafe() const;

    void RequestUnloadAsync();

private:
    // UI lifecycle
    void EnsureUiStarted();
    void ProcessUnloadIfRequested();

    // Aplicar cambios pendientes
    void ApplyPendingModeIfAny();
    void ApplyPendingBaseDirIfAny();
    void ApplyPendingVrxIfAny();

    // Archivo IQ
    std::string BuildTimestamp();
    std::string ModeToString(Mode m);
    std::string BuildBaseDataDir();
    void RotateIqFile(Mode mode);
    void CloseIqFile();
    void AppendIq(const std::vector<float>& iq);

    // Registro de stream processor
    void RebindStreamProcessor(int vrx);
    void UnbindStreamProcessor();

    // Métricas
    void UpdateReference(const std::vector<float>& iq);
    float CalculateRC(const std::vector<float>& iq);
    float CalculateINR(const std::vector<float>& iq);
    float CalculateLF(float rc, float inr);
    float CalculateRDE(float rc, float inr);
    std::string DetectPalimpsesto(const std::vector<float>& iq);
    void UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);
    void LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg);

private:
    IUnoPluginController& m_controller;
    std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;

    std::atomic<bool> m_uiStarted{false};
    std::atomic<bool> m_isStreaming{false};
    std::atomic<bool> m_captureEnabled{false};
    std::atomic<bool> m_streamProcRegistered{false};
    std::atomic<bool> m_unloadRequested{false};
    std::atomic<bool> m_isUnloading{false};
    std::atomic<bool> m_closingDown{false};

    int m_vrxIndex{0};

    // Config
    mutable std::mutex m_configMutex;
    std::string m_baseDir;
    std::string m_pendingBaseDir;
    std::atomic<bool> m_baseDirChangeRequested{false};

    Mode m_activeMode{Mode::Restrictivo};
    std::atomic<int> m_pendingMode{0}; // 0 Restrictivo, 1 Funcional
    std::atomic<bool> m_modeChangeRequested{false};

    std::atomic<int> m_pendingVrxIndex{0};
    std::atomic<bool> m_vrxChangeRequested{false};

    // Archivos
    std::ofstream m_iqOut;
    std::string m_currentFilePath;

    // Métricas
    std::vector<float> refSignal;
    bool haveRef{false};
    bool modoRestrictivo{true};
    std::chrono::steady_clock::time_point m_lastTick;

    // Logging
    std::ofstream logFile;
};
