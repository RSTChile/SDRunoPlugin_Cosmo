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
#include <atomic>
#include <string>
#include "SDRunoPlugin_TemplateForm.h"

class SDRunoPlugin_Template;

// Gestor de la interfaz del plugin (crea y controla el formulario)
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

    // Métodos para el diálogo de configuración
    std::string GetBaseDir() const;
    void RequestChangeBaseDir(const std::string& path);
    void RequestChangeVrx(int vrxIndex);
    void SettingsDialogClosed();

private:
    SDRunoPlugin_Template& m_parent;
    std::thread m_thread;
    std::shared_ptr<SDRunoPlugin_TemplateForm> m_form;
    bool m_started{ false };
    std::atomic<bool> m_shutdown{ false };  // Flag to prevent double destruction
    std::mutex m_lock;
    std::mutex m_formMutex;  // Protects form creation/destruction
    IUnoPluginController& m_controller;
    std::string m_baseDir;  // Carpeta base para guardar datos
};
