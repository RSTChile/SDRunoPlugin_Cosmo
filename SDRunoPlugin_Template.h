#pragma once

#include "iunoplugin.h"
#include "iunostreamobserver.h"
#include "iunoplugincontroller.h"
#include "SDRunoPlugin_TemplateForm.h"
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoStreamObserver {
public:
    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    void StreamObserverProcess(channel_t channel, const Complex* buffer, int length) override;

    void LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg);

    float CalculateRC(const std::vector<float>& iq);
    float CalculateINR(const std::vector<float>& iq);
    float CalculateLF(float rc, float inr);
    float CalculateRDE(float rc, float inr);

    std::string DetectPalimpsesto(const std::vector<float>& iq);

    void SetModeRestrictivo(bool restrictivo);
    bool GetModeRestrictivo() const;

private:
    SDRunoPlugin_TemplateForm m_form;
    std::ofstream logFile;
    std::vector<float> refSignal;
    bool haveRef;
    bool modoRestrictivo;

    void UpdateReference(const std::vector<float>& iq);
};
