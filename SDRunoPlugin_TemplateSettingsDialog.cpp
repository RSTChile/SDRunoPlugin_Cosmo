#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"

SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form)
    : nana::form(owner_form, nana::size(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false))
    , m_parent(&parent)
{
    Setup();
}

SDRunoPlugin_TemplateSettingsDialog::~SDRunoPlugin_TemplateSettingsDialog()
{
}

void SDRunoPlugin_TemplateSettingsDialog::Setup()
{
    // Center by default: no host calls from GUI thread
    move(nana::API::make_center(dialogFormWidth, dialogFormHeight));
    size(nana::size(dialogFormWidth, dialogFormHeight));
    caption("SDRuno Plugin Cosmo - Settings");

    // Dark background
    this->bgcolor(nana::color(45, 45, 48));

    // Controls
    titleLbl.caption("Cosmo Plugin Configuration");
    titleLbl.fgcolor(nana::color(220, 220, 220));
    titleLbl.transparent(true);
    titleLbl.text_align(nana::align::center, nana::align_v::center);

    // Texto multilínea y auto-break
    infoLbl.caption("Settings and configuration options will be available here.\n(More options coming soon)");
    infoLbl.fgcolor(nana::color(180, 180, 180));
    infoLbl.transparent(true);

    closeBtn.caption("Close");
    closeBtn.bgcolor(nana::color(70, 70, 73));
    closeBtn.fgcolor(nana::color(220, 220, 220));

    closeBtn.events().click([this]() {
        close();
        if (m_parent) { m_parent->SettingsDialogClosed(); }
    });

    events().unload([this](const nana::arg_unload&) {
        if (m_parent) { m_parent->SettingsDialogClosed(); }
    });
}
