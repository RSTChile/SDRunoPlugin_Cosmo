#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/dragger.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include "iunoplugincontroller.h"
#include <string>

#define topBarHeight (27)
#define bottomBarHeight (8)
#define sideBorderWidth (8)
#define formWidth (297)
#define formHeight (240)

class SDRunoPlugin_TemplateUi;
class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateSettingsDialog;

class SDRunoPlugin_TemplateForm : public nana::form
{
public:
	SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller, SDRunoPlugin_TemplateUi& ui);		
	~SDRunoPlugin_TemplateForm();
	
	void UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);

private:
	void Setup();

	nana::picture bg_border{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	nana::picture bg_inner{ bg_border, nana::rectangle(sideBorderWidth, topBarHeight, formWidth - (2 * sideBorderWidth), formHeight - topBarHeight - bottomBarHeight) };
	nana::picture header_bar{ *this, true };
	nana::label title_bar_label{ *this, true };
	nana::dragger form_dragger;
	nana::label form_drag_label{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	nana::paint::image img_min_normal;
	nana::paint::image img_min_down;
	nana::paint::image img_close_normal;
	nana::paint::image img_close_down;
	nana::paint::image img_header;
	nana::picture close_button{ *this, nana::rectangle(0, 0, 20, 15) };
	nana::picture min_button{ *this, nana::rectangle(0, 0, 20, 15) };
	nana::label versionLbl{ *this, nana::rectangle(formWidth - 40, formHeight - 30, 30, 20) };

	nana::paint::image img_sett_normal;
	nana::paint::image img_sett_down;
	nana::picture sett_button{ *this, nana::rectangle(0, 0, 40, 15) };
	void SettingsButton_Click();
	void SettingsDialog_Closed();

	nana::label rcLabel{ *this, nana::rectangle(20, 60, 220, 22) };
	nana::label inrLabel{ *this, nana::rectangle(20, 90, 220, 22) };
	nana::label lfLabel{ *this, nana::rectangle(20, 120, 220, 22) };
	nana::label rdeLabel{ *this, nana::rectangle(20, 150, 220, 22) };
	nana::label msgLabel{ *this, nana::rectangle(20, 180, 220, 40) };

	nana::combox modeCombo{ *this, nana::rectangle(20, 20, 120, 22) };
	
	// Settings button
	nana::button settingsBtn{ *this, nana::rectangle(150, 20, 80, 22) };

	SDRunoPlugin_Template& m_parent;
	IUnoPluginController& m_controller;
	SDRunoPlugin_TemplateUi& m_ui;

	// Settings dialog instance
	std::shared_ptr<SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;
};
