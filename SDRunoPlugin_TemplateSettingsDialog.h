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

#include <iunoplugincontroller.h>

#define dialogFormWidth (297)
#define dialogFormHeight (240)

class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_TemplateSettingsDialog : public nana::form
{
public:
    SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateSettingsDialog();

private:
    void Setup();
    int LoadX();
    int LoadY();

    // UI elements with dark theme
    nana::label titleLbl{ *this, nana::rectangle(20, 20, 260, 30) };
    nana::label infoLbl{ *this, nana::rectangle(20, 60, 260, 20) };
    nana::button closeBtn{ *this, nana::rectangle(200, 180, 80, 30) };

    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;
};
