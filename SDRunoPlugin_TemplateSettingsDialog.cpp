#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateForm.h"
#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#include <shlobj.h>
#endif

using namespace nana;

namespace {
#ifdef _WIN32
    static std::string WideToUtf8(const std::wstring& w) {
        if (w.empty()) return {};
        int needed = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
        if (needed <= 0) return {};
        std::string out; out.resize((size_t)needed);
        int written = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), out.data(), needed, nullptr, nullptr);
        if (written <= 0) return {};
        return out;
    }
    static std::string SelectFolderWin32() {
        BROWSEINFOW bi{};
        bi.lpszTitle = L"Seleccione carpeta de guardado";
        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
        if (!pidl) return {};
        wchar_t path[MAX_PATH]{};
        bool ok = SHGetPathFromIDListW(pidl, path);
        CoTaskMemFree(pidl);
        if (!ok) return {};
        return WideToUtf8(path);
    }
#else
    static std::string SelectFolderWin32() { return {}; }
#endif
}

SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& ui, SDRunoPlugin_TemplateForm& owner, IUnoPluginController& controller)
    : m_ui(ui)
    , m_controller(controller)
    , m_form(owner, rectangle(0, 0, 520, 360))
    , m_vrxList(m_form)
    , m_btnRefresh(m_form, "Refresh")
    , m_btnClose(m_form, "Close")
    , m_btnFolder(m_form, "Folder...")
    , m_title(m_form, "VRX disponibles")
    , m_folderLabel(m_form, "")
{
    BuildUi();
    PopulateVrxList();
    UpdateFolderLabel(m_ui.GetBaseDir());
}

void SDRunoPlugin_TemplateSettingsDialog::BuildUi()
{
    m_form.caption("Cosmo - Settings");
    m_title.move(rectangle{ 10, 10, 200, 24 });

    m_vrxList.move(rectangle{ 10, 40, 500, 220 });
    m_vrxList.append_header("VRX", 120);
    m_vrxList.append_header("Enabled", 120);

    m_btnRefresh.move(rectangle{ 10, 270, 100, 30 });
    m_btnFolder.move(rectangle{ 120, 270, 100, 30 });
    m_btnClose.move(rectangle{ 410, 270, 100, 30 });

    m_folderLabel.move(rectangle{ 10, 310, 500, 24 });

    m_btnRefresh.events().click([this]() { PopulateVrxList(); });
    m_btnFolder.events().click([this]() { OnPickFolder(); });
    m_btnClose.events().click([this]() { this->close(); });

    // Handle window close button (X)
    m_form.events().unload([this](const nana::arg_unload&) {
        this->close();
    });

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

void SDRunoPlugin_TemplateSettingsDialog::OnPickFolder()
{
#ifdef _WIN32
    std::string path = SelectFolderWin32();
    if (!path.empty()) {
        m_ui.RequestChangeBaseDir(path);
        UpdateFolderLabel(path);
    }
#else
    // En otras plataformas, de momento no-op
#endif
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

void SDRunoPlugin_TemplateSettingsDialog::UpdateFolderLabel(const std::string& path)
{
    m_folderLabel.caption(std::string("Folder: ") + path);
}

void SDRunoPlugin_TemplateSettingsDialog::show()
{
    m_form.show();
    PopulateVrxList();
    UpdateFolderLabel(m_ui.GetBaseDir());
}

void SDRunoPlugin_TemplateSettingsDialog::close()
{
    try { m_form.close(); } catch (...) {}
    try { m_ui.SettingsDialogClosed(); } catch (...) {}
}

nana::window SDRunoPlugin_TemplateSettingsDialog::handle() const
{
    return m_form.handle();
}
