#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iunoplugincontroller.h>
#include <memory>
#include <mutex>
#include <atomic>
#include <string>

class SDRunoPlugin_TemplateUi;

// Formulario principal del plugin
class SDRunoPlugin_TemplateForm : public nana::form
{
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateForm();

    void Run();
    void SetLedState(bool signalPresent);

    int GetLoadX();
    int GetLoadY();

private:
    void Setup();
    void OnFormClose();
    void OnSettingsClick();
    void UpdateSignalLed();
    
    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;
    
    // UI elements
    std::unique_ptr<nana::label> m_titleLabel;
    std::unique_ptr<nana::label> m_versionLabel;
    std::unique_ptr<nana::panel> m_signalLed;
    std::unique_ptr<nana::label> m_signalLabel;
    std::unique_ptr<nana::button> m_settingsButton;
    
    // State
    std::atomic<bool> m_signalPresent{false};
    std::mutex m_uiMutex;
    
    // Timer for UI updates
    std::unique_ptr<nana::timer> m_uiTimer;
};