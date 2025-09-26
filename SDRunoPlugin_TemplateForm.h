#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/dragger.hpp>
#include <nana/paint/image.hpp>
#include <iunoplugin.h>
#include <iunoplugincontroller.h>

class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_TemplateForm : public nana::form
{
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateForm();

    void Run();
    void Setup();
    void SetLedState(bool on);
    void SettingsButton_Click();
    void SettingsDialog_Closed();

    // Form constants
    static constexpr int formWidth = 400;
    static constexpr int formHeight = 200;
    static constexpr int sideBorderWidth = 8;
    static constexpr int topBarHeight = 30;

private:
    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;

    // Images for UI elements
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

    // UI Elements
    nana::picture bg_border{*this};
    nana::picture bg_inner{*this};
    nana::picture header_bar{*this};
    nana::button close_button{*this};
    nana::button min_button{*this};
    nana::button sett_button{*this};
    nana::label title_bar_label{*this};
    nana::label versionLbl{*this};
    nana::label form_drag_label{*this};
    nana::picture ledPicture{*this};

    // Form dragger for moving window
    nana::dragger form_dragger;
};