#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"

// Constructor con UI padre y owner form
SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form)
    : nana::form(owner_form, nana::size(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false))
    , m_parent(&parent)
{
    Setup();
}

SDRunoPlugin_TemplateSettingsDialog::~SDRunoPlugin_TemplateSettingsDialog()
{
    // Nana limpia sus manejadores al destruir controles
}

void SDRunoPlugin_TemplateSettingsDialog::Setup()
{
    // Centrar por defecto: no se leen posiciones desde el host (evita llamadas cross-thread)
    move(nana::API::make_center(dialogFormWidth, dialogFormHeight));
    size(nana::size(dialogFormWidth, dialogFormHeight));
    caption("SDRuno Plugin Cosmo - Settings");

    // Fondo estilo oscuro
    this->bgcolor(nana::color(45, 45, 48));

    // Controles
    titleLbl.caption("Cosmo Plugin Configuration");
    titleLbl.fgcolor(nana::color(220, 220, 220));
    titleLbl.transparent(true);
    titleLbl.text_align(nana::align::center, nana::align_v::center);

    infoLbl.caption("Settings and configuration options will be available here.");
    infoLbl.fgcolor(nana::color(180, 180, 180));
    infoLbl.transparent(true);

    closeBtn.caption("Close");
    closeBtn.bgcolor(nana::color(70, 70, 73));
    closeBtn.fgcolor(nana::color(220, 220, 220));

    closeBtn.events().click([this]() {
        close();
        if (m_parent) {
            m_parent->SettingsDialogClosed();
        }
    });

    // Avisar al padre cuando se cierra la ventana (por X o Alt+F4)
    events().unload([this](const nana::arg_unload&) {
        if (m_parent) {
            m_parent->SettingsDialogClosed();
        }
    });
}
