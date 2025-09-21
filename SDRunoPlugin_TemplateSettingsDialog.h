#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/dragger.hpp>
#include <memory>
#include <string>

class SDRunoPlugin_TemplateUi;

#define dialogFormWidth (297)
#define dialogFormHeight (240)

// Dialogo de Settings SIN acceso a IUnoPluginController desde el hilo GUI
class SDRunoPlugin_TemplateSettingsDialog : public nana::form
{
public:
    // Constructor con UI padre y owner form (no se usa controller aquí)
    SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form);
    ~SDRunoPlugin_TemplateSettingsDialog();

private:
    void Setup();

    // UI elements with dark theme
    nana::label titleLbl{ *this, nana::rectangle(20, 20, 260, 30) };
    nana::label infoLbl{ *this, nana::rectangle(20, 60, 260, 20) };
    nana::button closeBtn{ *this, nana::rectangle(200, 180, 80, 30) };

    SDRunoPlugin_TemplateUi* m_parent{nullptr}; // optional pointer to UI parent
};
