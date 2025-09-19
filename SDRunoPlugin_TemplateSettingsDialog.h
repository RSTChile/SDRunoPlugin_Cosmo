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
    void Run();

private:
    void Setup();
    int LoadX();
    int LoadY();

    // NUEVO: etiqueta para que no se vea vacío
    nana::label infoLbl{ *this, nana::rectangle(20, 20, 260, 20) };

    SDRunoPlugin_TemplateUi& m_parent;
    IUnoPluginController& m_controller;
};
