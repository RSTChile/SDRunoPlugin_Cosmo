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

#define formWidth  (420)
#define formHeight (260)

class SDRunoPlugin_TemplateForm : public nana::form {
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller, SDRunoPlugin_TemplateUi& ui);
    ~SDRunoPlugin_TemplateForm();

    void Run();
    void UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);
    void SettingsButton_Click(); // legado
    void SetSavePath(const std::string& path);

private:
    void Setup();

    SDRunoPlugin_Template& m_parent;
    IUnoPluginController& m_controller;
    SDRunoPlugin_TemplateUi& m_ui;

    // Controles
    nana::label title_bar_label{ *this };
    nana::label versionLbl{ *this, nana::rectangle(formWidth - 40, formHeight - 20, 40, 16) };

    nana::label rcLabel{ *this, nana::rectangle(20, 60, 220, 20) };
    nana::label inrLabel{ *this, nana::rectangle(20, 85, 220, 20) };
    nana::label lfLabel{ *this, nana::rectangle(20, 110, 220, 20) };
    nana::label rdeLabel{ *this, nana::rectangle(20, 135, 220, 20) };
    nana::label msgLabel{ *this, nana::rectangle(20, 165, 380, 24) };

    nana::label savePathCaption{ *this, nana::rectangle(20, 195, 80, 18) };
    nana::label savePathValue{ *this, nana::rectangle(100, 195, 300, 18) };

    nana::combox modeCombo{ *this, nana::rectangle(20, 20, 200, 26) };
    nana::button settingsBtn{ *this, nana::rectangle(230, 20, 100, 26) };

    std::shared_ptr<SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;
};
