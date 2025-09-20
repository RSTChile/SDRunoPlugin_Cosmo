#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#include <shlobj.h>
#endif

#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"

// Form constructor con handles a parent y controller
SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller, nana::form& owner_form) :
    nana::nested_form(owner_form, nana::API::make_center(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false)),
    m_parent(parent),
    m_controller(controller)
{
    Setup();
}

SDRunoPlugin_TemplateSettingsDialog::~SDRunoPlugin_TemplateSettingsDialog()
{
    this->events().destroy.clear();
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
    if (posX != -1 && posY != -1) {
        move(posX, posY);
    }

    size(nana::size(dialogFormWidth, dialogFormHeight));
    caption("SDRuno Plugin Cosmo - Settings");

    // SDRuno-style dark theme
    this->bgcolor(nana::color(45, 45, 48));  // Dark gray background like SDRuno
    
    // Title label with larger, bold-style text
    titleLbl.caption("Cosmo Plugin Configuration");
    titleLbl.fgcolor(nana::color(220, 220, 220));  // Light gray text
    titleLbl.transparent(true);
    titleLbl.text_align(nana::align::center, nana::align_v::center);
    
    // Info label with settings description
    infoLbl.caption("Settings and configuration options will be available here.");
    infoLbl.fgcolor(nana::color(180, 180, 180));  // Slightly darker gray text
    infoLbl.transparent(true);
    
    // Close button with SDRuno-style appearance
    closeBtn.caption("Close");
    closeBtn.bgcolor(nana::color(70, 70, 73));     // Darker button background
    closeBtn.fgcolor(nana::color(220, 220, 220));  // Light text
    
    // Handle close button click
    closeBtn.events().click([this]() {
        close();
        m_parent.SettingsDialogClosed();
    });
    
    // Handle window close event
    events().unload([this](const nana::arg_unload& arg) {
        m_parent.SettingsDialogClosed();
    });
}
