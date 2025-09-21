#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateForm.h"

using namespace nana;

SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& ui, SDRunoPlugin_TemplateForm& owner, IUnoPluginController& controller)
    : m_ui(ui)
    , m_controller(controller)
    , m_form(owner, rectangle(0, 0, 420, 320))
    , m_vrxList(m_form)
    , m_btnRefresh(m_form, "Refresh")
    , m_btnClose(m_form, "Close")
    , m_title(m_form, "VRX disponibles")
{
    BuildUi();
    PopulateVrxList();
}

void SDRunoPlugin_TemplateSettingsDialog::BuildUi()
{
    m_form.caption("Cosmo - Settings");
    m_title.move(rectangle{ 10, 10, 200, 24 });

    m_vrxList.move(rectangle{ 10, 40, 400, 220 });
    m_vrxList.append_header("VRX", 120);
    m_vrxList.append_header("Enabled", 120);

    m_btnRefresh.move(rectangle{ 10, 270, 100, 30 });
    m_btnClose.move(rectangle{ 310, 270, 100, 30 });

    m_btnRefresh.events().click([this]() { PopulateVrxList(); });
    m_btnClose.events().click([this]() { this->close(); });

    // Doble click: elegir VRX
    m_vrxList.events().dbl_click([this](const arg_mouse&) {
        auto sel = m_vrxList.selected();
        if (!sel.empty()) {
            auto ip = sel.front();
            try {
                auto txt = m_vrxList.at(ip.cat).at(ip.item).text(0);
                int vrxIndex = std::stoi(txt);
                m_ui.RequestChangeVrx(vrxIndex);
            } catch (...) {
                // Ignorar parseos fallidos
            }
        }
    });
}

void SDRunoPlugin_TemplateSettingsDialog::PopulateVrxList()
{
    m_vrxList.clear();
    int count = 0;
    try { count = m_controller.GetVRXCount(); } catch (...) { count = 0; }

    if (m_vrxList.size_categ() == 0) m_vrxList.append("VRX");

    auto cat = m_vrxList.at(0);
    for (int i = 0; i < count; ++i) {
        bool enabled = false;
        try { enabled = m_controller.GetVRXEnable(i); } catch (...) { enabled = false; }
        cat.append({ std::to_string(i), enabled ? "Yes" : "No" });
    }
}

void SDRunoPlugin_TemplateSettingsDialog::show()
{
    m_form.show();
    // Refrescar al abrir
    PopulateVrxList();
}

void SDRunoPlugin_TemplateSettingsDialog::close()
{
    try { m_form.close(); } catch (...) {}
    // Avisar al UI para liberar el shared_ptr
    try { m_ui.SettingsDialogClosed(); } catch (...) {}
}

nana::native_window_type SDRunoPlugin_TemplateSettingsDialog::handle() const
{
    return m_form.handle();
}
