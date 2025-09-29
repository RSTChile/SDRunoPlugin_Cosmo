#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iunoplugincontroller.h>
#include <memory>
#include <string>
#include <unoevent.h>

// Forward declarations
class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_TemplateForm : public nana::form
{
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);
    virtual ~SDRunoPlugin_TemplateForm();

    void HandleEvent(const UnoEvent& ev);
    void SetLedState(bool signalPresent);
    void UpdateMetrics(float rc, float inr, float lf, float rde);
    void SetMode(bool restrictive);
    void ShowSettingsDialog();

    int GetLoadX();
    int GetLoadY();

private:
    void Setup();
    void OnUnload();
    void OnSettingsClick();
    void OnModeChanged();
    void StartMetricsTimer();
    void UpdateUI();

    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;

    // UI Elements
    nana::label m_titleLabel;
    nana::label m_versionLabel;
    nana::label m_rcLabel;
    nana::label m_inrLabel;
    nana::label m_lfLabel;
    nana::label m_rdeLabel;
    nana::label m_ledLabel;
    nana::label m_modeLabel;
    nana::button m_settingsBtn;
    nana::button m_unloadBtn;
    nana::panel<true> m_ledPanel;

    // Timer for metrics update
    std::unique_ptr<nana::timer> m_timer;
    
    // State
    bool m_signalPresent;
    bool m_restrictiveMode;
    float m_lastRC, m_lastINR, m_lastLF, m_lastRDE;

    static const int FORM_WIDTH = 297;
    static const int FORM_HEIGHT = 240;
};