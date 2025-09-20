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
    m_parent(&parent),
    m_controller(controller)
{
    Setup();
}

// Form constructor only with controller (called from main form)
SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(IUnoPluginController& controller) :
    nana::form(nana::API::make_center(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false)),
    m_parent(nullptr),
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

    // Apply dark theme similar to SDRuno
    this->bgcolor(nana::colors::black);

    // Setup info label with improved styling
    infoLbl.caption("Configuración de Cosmo Plugin");
    infoLbl.fgcolor(nana::colors::white);
    infoLbl.transparent(true);
    infoLbl.text_align(nana::align::center, nana::align_v::center);
    
    // Add a subtitle
    auto subtitle = std::make_shared<nana::label>(*this, nana::rectangle(20, 50, 260, 20));
    subtitle->caption("Próximamente: configuraciones avanzadas");
    subtitle->fgcolor(nana::colors::light_gray);
    subtitle->transparent(true);
    subtitle->text_align(nana::align::center, nana::align_v::center);
    
    // Handle close event
    this->events().unload([this](const nana::arg_unload&) {
        // Settings dialog is being closed
        if (m_parent) {
            // If called from UI manager, notify it
            // Currently we don't have a direct callback, but the shared_ptr will be reset
        }
    });
}
