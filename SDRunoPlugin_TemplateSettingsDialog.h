#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/dragger.hpp>
#include <memory>
#include <string>

class SDRunoPlugin_TemplateUi;

#define dialogFormWidth (360)
#define dialogFormHeight (260)

// Settings dialog WITHOUT accessing IUnoPluginController from GUI thread
class SDRunoPlugin_TemplateSettingsDialog : public nana::form
{
public:
    // Constructor with UI parent and owner form
    SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& parent, nana::form& owner_form);
    ~SDRunoPlugin_TemplateSettingsDialog();

private:
    void Setup();

    // UI elements with dark theme
    nana::label titleLbl{ *this, nana::rectangle(20, 20, 320, 28) };
    nana::label infoLbl{ *this, nana::rectangle(20, 60, 320, 60) };
    nana::button closeBtn{ *this, nana::rectangle(260, 200, 80, 30) };

    SDRunoPlugin_TemplateUi* m_parent{nullptr};
};
