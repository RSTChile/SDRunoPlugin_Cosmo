#pragma once

#include <memory>
#include <mutex>
#include "iunoplugincontroller.h"
#include "SDRunoPlugin_TemplateForm.h"

class SDRunoPlugin_Template;

// UI principal para el plugin Cosmo
class SDRunoPlugin_TemplateUi {
public:
    SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateUi();

    void ShowUi();
    int LoadX();
    int LoadY();
    void HandleEvent(const UnoEvent& ev);
    void FormClosed();

private:
    SDRunoPlugin_Template& m_parent; // CORREGIDO: referencia al plugin, no a la UI
    std::shared_ptr<SDRunoPlugin_TemplateForm> m_form;
    IUnoPluginController& m_controller;
    std::mutex m_lock;
};
