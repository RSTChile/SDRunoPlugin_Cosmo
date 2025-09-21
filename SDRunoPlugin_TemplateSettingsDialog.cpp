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
                        // Convertir a UTF-8
                        int size = WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, nullptr, 0, nullptr, nullptr);
                        result.resize(size > 0 ? size - 1 : 0);
                        if (size > 1) WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, result.data(), size - 1, nullptr, nullptr);
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

SDRunoPlugin_TemplateSettingsDialog::SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form)
    : nana::form(owner_form, nana::size(dialogFormWidth, dialogFormHeight), nana::appearance(true, false, true, false, false, false, false))
    , m_parent(&parent)
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

void SDRunoPlugin_TemplateSettingsDialog::Setup()
{
    // Center by default
    move(nana::API::make_center(dialogFormWidth, dialogFormHeight));
    size(nana::size(dialogFormWidth, dialogFormHeight));
    caption("SDRuno Plugin Cosmo - Settings");

    // Dark background
    this->bgcolor(nana::color(45, 45, 48));

    titleLbl.caption("Cosmo Plugin Configuration");
    titleLbl.fgcolor(nana::color(220, 220, 220));
    titleLbl.transparent(true);
    titleLbl.text_align(nana::align::center, nana::align_v::center);

    infoLbl.caption("Configure capture folder and VRX source.\nChanges apply immediately for new files and stream attach.");
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
    vrxCombo.push_back("0");
    vrxCombo.push_back("1");
    vrxCombo.push_back("2");
    vrxCombo.push_back("3");
    vrxCombo.option(0);

    applyVrxBtn.caption("Apply");
    applyVrxBtn.events().click([this]() {
        int vrx = vrxCombo.option(); // 0..3
        if (m_parent) m_parent->RequestChangeVrx(vrx);
    });

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
