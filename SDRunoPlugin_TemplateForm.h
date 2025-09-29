#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iunoplugincontroller.h>

class SDRunoPlugin_TemplateUi;

// Form window for the plugin
class SDRunoPlugin_TemplateForm : public nana::form
{
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateForm();

    void Run();
    void Setup();
    void SetLedState(bool signalPresent);

private:
    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;
    
    // UI elements would go here
    // nana::label m_ledLabel;
    // nana::button m_settingsButton;
    // etc.
};