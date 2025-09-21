#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <memory>
#include <string>
#include <vector>

class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateUi;
class SDRunoPlugin_TemplateSettingsDialog;
class IUnoPluginController;

#define formWidth  (480)
#define formHeight (280)

class SDRunoPlugin_TemplateForm : public nana::form {
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller, SDRunoPlugin_TemplateUi& ui);
    ~SDRunoPlugin_TemplateForm();

    void Run();
    void UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);
    void SettingsButton_Click(); // legado
    void SetSavePath(const std::string& path);
    void SetStreaming(bool streaming);

private:
    void Setup();
    void CreateLedBars();
    void UpdateBar(std::vector<std::unique_ptr<nana::panel<true>>>& bar, float value, const nana::color& on, const nana::color& off);

    SDRunoPlugin_Template& m_parent;
    IUnoPluginController& m_controller;
    SDRunoPlugin_TemplateUi& m_ui;

    // Controles
    nana::label title_bar_label{ *this };
    nana::label versionLbl{ *this, nana::rectangle(formWidth - 40, formHeight - 20, 40, 16) };

    nana::label rcLabel{ *this, nana::rectangle(20, 60, 60, 20) };
    nana::label inrLabel{ *this, nana::rectangle(20, 90, 60, 20) };
    nana::label lfLabel{ *this, nana::rectangle(20, 120, 60, 20) };
    nana::label rdeLabel{ *this, nana::rectangle(20, 150, 60, 20) };
    nana::label msgLabel{ *this, nana::rectangle(20, 180, 440, 24) };

    nana::label estadoCaption{ *this, nana::rectangle(20, 215, 60, 18) };
    nana::label estadoValor{ *this, nana::rectangle(85, 215, 375, 18) };

    nana::combox modeCombo{ *this, nana::rectangle(20, 20, 220, 26) };
    nana::button settingsBtn{ *this, nana::rectangle(250, 20, 100, 26) };

    // LED bars
    static constexpr int LEDS = 20;
    static constexpr int ledSize = 10;
    static constexpr int ledGap = 2;
    int barStartX = 90;
    int barWidth = LEDS * (ledSize + ledGap);

    std::vector<std::unique_ptr<nana::panel<true>>> rcBar;
    std::vector<std::unique_ptr<nana::panel<true>>> inrBar;
    std::vector<std::unique_ptr<nana::panel<true>>> lfBar;
    std::vector<std::unique_ptr<nana::panel<true>>> rdeBar;

    std::shared_ptr<SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;

    // Estado
    bool m_streaming{false};
    std::string m_currentPath;
};
