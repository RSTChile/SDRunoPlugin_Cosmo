#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/dragger.hpp>
#include <nana/paint/image.hpp>
#include <iunoplugincontroller.h>

class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_TemplateForm : public nana::form
{
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);
    virtual ~SDRunoPlugin_TemplateForm();

    void Run();
    void Setup();
    void SetLedState(bool on);
    void SettingsButton_Click();
    void SettingsDialog_Closed();

private:
    // Constants
    static const int formWidth = 504;
    static const int formHeight = 350;
    static const int sideBorderWidth = 8;
    static const int topBarHeight = 50;

    // References
    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;

    // UI Images
    nana::paint::image img_bg_border;
    nana::paint::image img_bg_inner;
    nana::paint::image img_header;
    nana::paint::image img_min_normal;
    nana::paint::image img_min_down;
    nana::paint::image img_close_normal;
    nana::paint::image img_close_down;
    nana::paint::image img_sett_normal;
    nana::paint::image img_sett_down;
    nana::paint::image ledOnImg;
    nana::paint::image ledOffImg;

    // UI Controls
    nana::picture bg_border{*this};
    nana::picture bg_inner{*this};
    nana::picture header_bar{*this};
    nana::picture close_button{*this};
    nana::picture min_button{*this};
    nana::picture sett_button{*this};
    nana::picture ledPicture{*this};
    nana::label title_bar_label{*this};
    nana::label versionLbl{*this};
    nana::label form_drag_label{*this};

    // Dragger for window movement
    nana::dragger form_dragger;
};