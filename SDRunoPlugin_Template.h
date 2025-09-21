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

    void RequestUnloadAsync();

private:
    void EnsureUiStarted();

private:
    std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;
    std::atomic<bool> m_uiStarted{false};
    std::ofstream logFile;
    std::vector<float> refSignal;
    bool haveRef{false};
    bool modoRestrictivo{true};

    std::atomic<bool> m_unloadRequested{false};
    std::atomic<bool> m_isUnloading{false};
    std::atomic<bool> m_closingDown{false};

    void UpdateReference(const std::vector<float>& iq);
};
