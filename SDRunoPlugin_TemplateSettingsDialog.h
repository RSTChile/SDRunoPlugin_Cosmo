#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <memory>
#include "iunoplugincontroller.h"

class SDRunoPlugin_TemplateUi;
class SDRunoPlugin_TemplateForm;

class SDRunoPlugin_TemplateSettingsDialog {
public:
    SDRunoPlugin_TemplateSettingsDialog(SDRunoPlugin_TemplateUi& ui, SDRunoPlugin_TemplateForm& owner, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateSettingsDialog() = default;

    void show();
    void close();

    nana::window handle() const;

private:
    void BuildUi();
    void PopulateVrxList();
    void UpdateFolderLabel(const std::string& path);
    void OnPickFolder();

    SDRunoPlugin_TemplateUi& m_ui;
    IUnoPluginController& m_controller;

    nana::form    m_form;
    nana::listbox m_vrxList;
    nana::button  m_btnRefresh;
    nana::button  m_btnClose;
    nana::button  m_btnFolder;
    nana::label   m_title;
    nana::label   m_folderLabel;
};
