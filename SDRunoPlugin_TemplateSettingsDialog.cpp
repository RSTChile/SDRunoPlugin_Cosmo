#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"
#include <io.h>
#include <shlobj.h>

// Form constructor con handles a parent y controller
SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller) :
    nana::form(nana::API::make_center(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false)),
    m_parent(parent),
    m_controller(controller)
{
    Setup();
}

SDRunoPlugin_TemplateSettingsDialog::~SDRunoPlugin_TemplateSettingsDialog()
{
    this->events().destroy.clear();
}

void SDRunoPlugin_TemplateSettingsDialog::Run()
{
    show();
    nana::exec();
}

int SDRunoPlugin_TemplateSettingsDialog::LoadX()
{
    std::string tmp;
    m_controller.GetConfigurationKey("Template.Settings.X", tmp);
    if (tmp.empty()) { return -1; }
    return stoi(tmp);
}

int SDRunoPlugin_TemplateSettingsDialog::LoadY()
{
    std::string tmp;
    m_controller.GetConfigurationKey("Template.Settings.Y", tmp);
    if (tmp.empty()) { return -1; }
    return stoi(tmp);
}

void SDRunoPlugin_TemplateSettingsDialog::Setup()
{
    int posX = LoadX();
    int posY = LoadY();
    move(posX, posY);

    size(nana::size(dialogFormWidth, dialogFormHeight));
    caption("SDRuno Plugin Cosmo - Settings");

    this->bgcolor(nana::colors::black);

    // NUEVO: muestra un texto para que no se vea vacío
    infoLbl.caption("Settings de Cosmo (próximamente)");
    infoLbl.fgcolor(nana::colors::white);
    infoLbl.transparent(true);
}
