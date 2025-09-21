#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/dragger.hpp>
#include <memory>
#include <string>

class SDRunoPlugin_TemplateUi;

#define dialogFormWidth (460)
#define dialogFormHeight (260)

// Settings dialog SIN acceder a IUnoPluginController desde el hilo GUI
class SDRunoPlugin_TemplateSettingsDialog : public nana::form
{
public:
    // Constructor con padre UI y form propietario
    SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form);
    ~SDRunoPlugin_TemplateSettingsDialog();

private:
    void Setup();
    void RefreshBaseDir();

    // Controles UI con tema oscuro
    nana::label titleLbl{ *this, nana::rectangle(20, 16, 420, 24) };
    nana::label infoLbl{ *this, nana::rectangle(20, 46, 420, 40) };

    // Carpeta
    nana::label folderCaption{ *this, nana::rectangle(20, 96, 100, 20) };
    nana::label folderValue{ *this, nana::rectangle(120, 96, 320, 20) };
    nana::button changeFolderBtn{ *this, nana::rectangle(20, 122, 120, 26) };

    // VRX
    nana::label vrxCaption{ *this, nana::rectangle(20, 160, 100, 20) };
    nana::combox vrxCombo{ *this, nana::rectangle(120, 160, 80, 24) };
    nana::button applyVrxBtn{ *this, nana::rectangle(210, 160, 80, 24) };

    nana::button closeBtn{ *this, nana::rectangle(360, 200, 80, 30) };

    SDRunoPlugin_TemplateUi* m_parent{nullptr};
};
