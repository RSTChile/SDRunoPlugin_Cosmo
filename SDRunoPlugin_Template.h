#pragma once
#include "iunoplugin.h"
#include "iunostreamobserver.h"
#include <memory>
#include <atomic>
#include <fstream>
#include <vector>
#include <string>

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

    // Signal from GUI thread: request unload safely (will run on plugin thread)
    void RequestUnloadAsync();

private:
    // Deferred UI creation
    void EnsureUiStarted();

private:
    std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;
    std::atomic<bool> m_uiStarted{false};
    std::ofstream logFile;
    std::vector<float> refSignal;
    bool haveRef{false};
    bool modoRestrictivo{true};

    // Unload coordination
    std::atomic<bool> m_unloadRequested{false}; // consumed on plugin thread
    std::atomic<bool> m_isUnloading{false};     // set once per lifecycle
    std::atomic<bool> m_closingDown{false};     // set on ClosingDown event

    void UpdateReference(const std::vector<float>& iq);
};
