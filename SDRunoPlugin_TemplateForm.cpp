#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"

#define formWidth 297
#define formHeight 240
#define VERSION "V1.1"

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(
    SDRunoPlugin_TemplateUi& parent,
    IUnoPluginController& controller)
    : nana::form(nana::API::make_center(formWidth, formHeight),
                 nana::appearance(false, true, false, false, true, false, false)),
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

// Aquí iría el resto de la implementación, siguiendo el diseño original (Setup, eventos, etc.).