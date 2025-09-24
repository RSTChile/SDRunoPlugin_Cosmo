#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/dragger.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

#include <iunoplugincontroller.h>

#define topBarHeight (27)
#define bottomBarHeight (8)
#define sideBorderWidth (8)
#define formWidth (297)
#define formHeight (240)

class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_TemplateForm : public nana::form
{
public:
    SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);        
    ~SDRunoPlugin_TemplateForm();
    
    void Run();
    void SetLedState(bool on);

private:
    void Setup();

    nana::picture bg_border{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
    nana::picture bg_inner{ bg_border, nana::rectangle(sideBorderWidth, topBarHeight, formWidth - (2 * sideBorderWidth), formHeight - topBarHeight - bottomBarHeight) };
    nana::picture header_bar{ *this, true };
    nana::label title_bar_label{ *this, true };
    nana::dragger form_dragger;
    nana::label form_drag_label{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
    nana::paint::image img_min_normal;
    nana::paint::image img_min_down;
    nana::paint::image img_close_normal;
    nana::paint::image img_close_down;
    nana::paint::image img_header;
    nana::picture close_button{ *this, nana::rectangle(0, 0, 20, 15) };
    nana::picture min_button{ *this, nana::rectangle(0, 0, 20, 15) };
    nana::label versionLbl{ *this, nana::rectangle(formWidth - 40, formHeight - 30, 30, 20) };

    nana::paint::image img_sett_normal;
    nana::paint::image img_sett_down;
    nana::picture sett_button{ *this, nana::rectangle(0, 0, 40, 15) };
    void SettingsButton_Click();
    void SettingsDialog_Closed();

    // LED
    nana::picture ledPicture{ *this, nana::rectangle(20, 40, 20, 20) };
    nana::paint::image ledOnImg, ledOffImg;

    SDRunoPlugin_TemplateUi & m_parent;
    IUnoPluginController & m_controller;

    // Settings dialog management
    std::unique_ptr<class SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;
};
