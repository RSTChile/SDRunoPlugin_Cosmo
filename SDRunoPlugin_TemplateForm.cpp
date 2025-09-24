#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"
#include <io.h>
#include <shlobj.h>

#define VERSION "V1.1"

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller) :
    nana::form(nana::API::make_center(formWidth, formHeight), nana::appearance(false, true, false, false, true, false, false)),
    m_parent(parent),
    m_controller(controller)
{
    Setup();
}

SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm()
{
}

void SDRunoPlugin_TemplateForm::Run()
{
    show();
    nana::exec();
}

void SDRunoPlugin_TemplateForm::Setup()
{
    // ... (código de setup original)

    // LED
    ledOnImg.open("..\\resources\\led_on.bmp");
    ledOffImg.open("..\\resources\\led_off.bmp");
    ledPicture.load(ledOffImg, nana::rectangle(0, 0, 20, 20));
    ledPicture.transparent(true);

    // ... (resto del código de setup original)
}

void SDRunoPlugin_TemplateForm::SetLedState(bool on)
{
    if (on)
        ledPicture.load(ledOnImg, nana::rectangle(0, 0, 20, 20));
    else
        ledPicture.load(ledOffImg, nana::rectangle(0, 0, 20, 20));
}

void SDRunoPlugin_TemplateForm::SettingsButton_Click()
{
    SDRunoPlugin_TemplateSettingsDialog settingsDialog{ m_parent,m_controller };
    this->enabled(false);
    settingsDialog.events().unload([&] { SettingsDialog_Closed(); });
    settingsDialog.Run();
}

void SDRunoPlugin_TemplateForm::SettingsDialog_Closed()
{
    this->enabled(true);
    this->focus();
}
