#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iunoplugincontroller.h>
#include <memory>
#include <thread>
#include <mutex>
#include <string>

// Forward declarations
class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateForm;

// UI manager class that handles window lifecycle
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
    void UpdateMetrics(float rc, float inr, float lf, float rde);

    // Methods for configuration dialog
    std::string GetBaseDir() const;
    void RequestChangeBaseDir(const std::string& path);
    void RequestChangeVrx(int vrxIndex);
    void SettingsDialogClosed();

private:
    SDRunoPlugin_Template& m_parent;
    std::thread m_thread;
    std::shared_ptr<SDRunoPlugin_TemplateForm> m_form;
    bool m_started;
    std::mutex m_lock;
    IUnoPluginController& m_controller;
    std::string m_baseDir;  // Base folder for saving data
};
