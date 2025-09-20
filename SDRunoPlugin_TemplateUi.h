#pragma once

#include <memory>
#include <mutex>
#include "iunoplugincontroller.h"

class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateForm;
class SDRunoPlugin_TemplateSettingsDialog;

// UI principal para el plugin Cosmo
class SDRunoPlugin_TemplateUi {
public:
    SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateUi();

    void ShowMainWindow();
    void ShowSettingsDialog();
    void UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);
    
    int LoadX();
    int LoadY();
    void HandleEvent(const UnoEvent& ev);
    void FormClosed();
    void SettingsDialogClosed();

private:
    SDRunoPlugin_Template& m_parent;
    std::shared_ptr<SDRunoPlugin_TemplateForm> m_mainForm;
    std::shared_ptr<SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;
    IUnoPluginController& m_controller;
    std::mutex m_lock;
    
    void StartMainLoop();
};
