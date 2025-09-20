#pragma once

#include "iunoplugin.h"
#include "iunostreamobserver.h"
#include "iunoplugincontroller.h"
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <memory>

// Forward declarations
class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoStreamObserver {
public:
    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    // IUnoPlugin overrides
    void HandleEvent(const UnoEvent& ev) override;

    void StreamObserverProcess(channel_t channel, const Complex* buffer, int length) override;

    void LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg);

    float CalculateRC(const std::vector<float>& iq);
    float CalculateINR(const std::vector<float>& iq);
    float CalculateLF(float rc, float inr);
    float CalculateRDE(float rc, float inr);

    std::string DetectPalimpsesto(const std::vector<float>& iq);

    void SetModeRestrictivo(bool restrictivo);
    bool GetModeRestrictivo() const;

    // UI Management
    void UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);

private:
    std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;
    std::ofstream logFile;
    std::vector<float> refSignal;
    bool haveRef;
    bool modoRestrictivo;

    void UpdateReference(const std::vector<float>& iq);
};
