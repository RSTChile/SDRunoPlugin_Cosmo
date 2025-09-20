#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#include <shlobj.h>
#endif

#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"

// Constructor para diálogo anidado con padre UI, forma anidada y controller
SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller, nana::form& owner_form) :
    nana::nested_form(owner_form, nana::API::make_center(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false)),
    m_parent(parent),
    m_controller(controller)
{
    Setup();
}

// Constructor para diálogo stand-alone solo con controller (alternativo)
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

    // Fondo estilo oscuro similar a SDRuno
    this->bgcolor(nana::color(45, 45, 48));

    // Etiqueta título con texto claro y centrado
    titleLbl.caption("Cosmo Plugin Configuration");
    titleLbl.fgcolor(nana::color(220, 220, 220));
    titleLbl.transparent(true);
    titleLbl.text_align(nana::align::center, nana::align_v::center);

    // Etiqueta de información sobre configuración
    infoLbl.caption("Settings and configuration options will be available here.");
    infoLbl.fgcolor(nana::color(180, 180, 180));
    infoLbl.transparent(true);

    // Botón cerrar con estilo SDRuno
    closeBtn.caption("Close");
    closeBtn.bgcolor(nana::color(70, 70, 73));
    closeBtn.fgcolor(nana::color(220, 220, 220));

    closeBtn.events().click([this]() {
        close();
        if (m_parent) {
            m_parent.SettingsDialogClosed();
        }
    });

    // Manejar evento de cierre de ventana para avisar al padre
    events().unload([this](const nana::arg_unload& arg) {
        if (m_parent) {
            m_parent.SettingsDialogClosed();
        }
    });
}
