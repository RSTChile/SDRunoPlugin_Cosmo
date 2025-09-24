#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <memory>

#include <iunoplugincontroller.h>

class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateForm;

class SDRunoPlugin_TemplateUi
{
public:
    SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateUi();

    void HandleEvent(const UnoEvent& evt);
    void FormClosed();

    void ShowUi();

    int LoadX();
    int LoadY();

    void UpdateLed(bool signalPresent);

    // Métodos faltantes agregados
    std::string GetBaseDir();
    void RequestChangeVrx(int vrxIndex);
    void RequestChangeBaseDir(const std::string& path);
    void SettingsDialogClosed();

private:
    SDRunoPlugin_Template & m_parent;
    std::thread m_thread;
    std::shared_ptr<SDRunoPlugin_TemplateForm> m_form;

    bool m_started;

    std::mutex m_lock;

    IUnoPluginController & m_controller;
};
