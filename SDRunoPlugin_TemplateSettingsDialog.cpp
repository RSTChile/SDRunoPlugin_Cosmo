#include <sstream>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <shobjidl.h>
#endif

#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"

static std::string PickFolderDialog(HWND owner) {
#ifdef _WIN32
    std::string result;
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    IFileDialog* pfd = nullptr;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
        if (SUCCEEDED(hr)) {
            DWORD opts;
            if (SUCCEEDED(pfd->GetOptions(&opts))) {
                pfd->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
            }
            if (SUCCEEDED(pfd->Show(owner))) {
                IShellItem* psi = nullptr;
                if (SUCCEEDED(pfd->GetResult(&psi))) {
                    PWSTR pszPath = nullptr;
                    if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath))) {
                        int needed = WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, nullptr, 0, nullptr, nullptr);
                        if (needed > 1) {
                            std::string out(needed - 1, '\0');
                            WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, out.data(), needed - 1, nullptr, nullptr);
                            result = out;
                        }
                        CoTaskMemFree(pszPath);
                    }
                    psi->Release();
                }
            }
            pfd->Release();
        }
        CoUninitialize();
    }
    return result;
#else
    return std::string();
#endif
}

SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form, IUnoPluginController& controller)
    : nana::form(owner_form, nana::size(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false))
    , m_parent(&parent)
    , m_controller(&controller)
{
    Setup();
}

SDRunoPlugin_TemplateSettingsDialog::~SDRunoPlugin_TemplateSettingsDialog()
{
}

void SDRunoPlugin_TemplateSettingsDialog::RefreshBaseDir()
{
    if (!m_parent) return;
    folderValue.caption(m_parent->GetBaseDir());
}

void SDRunoPlugin_TemplateSettingsDialog::FillActiveVrxList()
{
    vrxCombo.clear();
    int selectedIdx = 0;

    if (m_controller) {
        int count = 0;
        try { count = m_controller->GetVRXCount(); } catch (...) { count = 0; }

        int firstActive = -1;
        for (int i = 0; i < count; ++i) {
            bool enabled = false;
            try { enabled = m_controller->GetVRXEnable(i); } catch (...) { enabled = false; }
            if (enabled) {
                if (firstActive < 0) firstActive = i;
                vrxCombo.push_back(std::to_string(i));
            }
        }

        if (vrxCombo.the_number_of_options() == 0) {
            // Si no hay VRX "activos", mostrar todos [0..count-1]
            for (int i = 0; i < count; ++i) vrxCombo.push_back(std::to_string(i));
            selectedIdx = 0;
        } else {
            // Seleccionar el primer activo
            selectedIdx = 0;
        }
    }

    if (vrxCombo.the_number_of_options() > 0) vrxCombo.option(selectedIdx);
}

void SDRunoPlugin_TemplateSettingsDialog::Setup()
{
    move(nana::API::make_center(dialogFormWidth, dialogFormHeight));
    size(nana::size(dialogFormWidth, dialogFormHeight));
    caption("SDRuno Plugin Cosmo - Settings");

    this->bgcolor(nana::color(45, 45, 48));

    titleLbl.caption("Cosmo Plugin Configuration");
    titleLbl.fgcolor(nana::color(220, 220, 220));
    titleLbl.transparent(true);
    titleLbl.text_align(nana::align::center, nana::align_v::center);

    infoLbl.caption("Select capture folder and choose an active VRX source.");
    infoLbl.fgcolor(nana::color(180, 180, 180));
    infoLbl.transparent(true);

    folderCaption.caption("Folder:");
    folderCaption.fgcolor(nana::color(200,200,200));
    RefreshBaseDir();

    changeFolderBtn.caption("Change Folder…");
    changeFolderBtn.bgcolor(nana::color(70, 70, 73));
    changeFolderBtn.fgcolor(nana::color(220, 220, 220));
    changeFolderBtn.events().click([this]() {
        auto hwnd = reinterpret_cast<HWND>(this->native_handle());
        auto chosen = PickFolderDialog(hwnd);
        if (!chosen.empty() && m_parent) {
            m_parent->RequestChangeBaseDir(chosen);
            folderValue.caption(chosen);
        }
    });

    vrxCaption.caption("VRX:");
    vrxCaption.fgcolor(nana::color(200,200,200));

    refreshVrxBtn.caption("Refresh");
    refreshVrxBtn.events().click([this]() { FillActiveVrxList(); });

    applyVrxBtn.caption("Apply");
    applyVrxBtn.events().click([this]() {
        if (!m_parent) return;
        int opt = vrxCombo.option();
        if (opt >= 0 && opt < static_cast<int>(vrxCombo.the_number_of_options())) {
            try {
                int vrx = std::stoi(vrxCombo.text(opt));
                m_parent->RequestChangeVrx(vrx);
            } catch (...) {}
        }
    });

    // Inicializar lista de VRX activos
    FillActiveVrxList();

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
