#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/dragger.hpp>
#include <memory>
#include <string>
#include "iunoplugincontroller.h"

class SDRunoPlugin_TemplateUi;

#define dialogFormWidth (520)
#define dialogFormHeight (280)

// Settings dialog: permite cambiar carpeta, seleccionar VRX activo
class SDRunoPlugin_TemplateSettingsDialog : public nana::form
{
public:
    // Constructor con padre UI, form propietario y controlador (para consultar VRX activos)
    SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateSettingsDialog();

private:
    void Setup();
    void RefreshBaseDir();
    void FillActiveVrxList();   // Lee VRX activos del controlador y llena combo

    // Controles
    nana::label titleLbl{ *this, nana::rectangle(20, 16, 480, 24) };
    nana::label infoLbl{ *this, nana::rectangle(20, 46, 480, 40) };

    // Carpeta
    nana::label folderCaption{ *this, nana::rectangle(20, 96, 100, 20) };
    nana::label folderValue{ *this, nana::rectangle(120, 96, 360, 20) };
    nana::button changeFolderBtn{ *this, nana::rectangle(20, 122, 140, 26) };

    // VRX
    nana::label vrxCaption{ *this, nana::rectangle(20, 160, 100, 20) };
    nana::combox vrxCombo{ *this, nana::rectangle(120, 160, 120, 24) };
    nana::button refreshVrxBtn{ *this, nana::rectangle(250, 160, 80, 24) };
    nana::button applyVrxBtn{ *this, nana::rectangle(340, 160, 80, 24) };

    nana::button closeBtn{ *this, nana::rectangle(420, 220, 80, 30) };

    SDRunoPlugin_TemplateUi* m_parent{nullptr};
    IUnoPluginController* m_controller{nullptr};
};
