#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <memory>
#include <string>

class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateUi;
class SDRunoPlugin_TemplateSettingsDialog;
class IUnoPluginController;

#define formWidth  (360)
#define formHeight (240)

class SDRunoPlugin_TemplateForm : public nana::form {
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller, SDRunoPlugin_TemplateUi& ui);
    ~SDRunoPlugin_TemplateForm();

    void Run();
    void UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);
    void SettingsButton_Click(); // legado

private:
    void Setup();

    SDRunoPlugin_Template& m_parent;
    IUnoPluginController& m_controller;
    SDRunoPlugin_TemplateUi& m_ui;

    // Controles
    nana::label title_bar_label{ *this };
    nana::label versionLbl{ *this, nana::rectangle(formWidth - 40, formHeight - 20, 40, 16) };

    nana::label rcLabel{ *this, nana::rectangle(20, 60, 200, 20) };
    nana::label inrLabel{ *this, nana::rectangle(20, 85, 200, 20) };
    nana::label lfLabel{ *this, nana::rectangle(20, 110, 200, 20) };
    nana::label rdeLabel{ *this, nana::rectangle(20, 135, 200, 20) };
    nana::label msgLabel{ *this, nana::rectangle(20, 165, 320, 24) };

    nana::combox modeCombo{ *this, nana::rectangle(20, 20, 180, 26) };
    nana::button settingsBtn{ *this, nana::rectangle(210, 20, 100, 26) };

    std::shared_ptr<SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;
};
