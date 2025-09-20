#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include <sstream>

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller)
	: nana::form(nana::API::make_center(formWidth, formHeight)),
	  m_parent(parent), m_controller(controller)
{
	Setup();
}

SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm() {}

void SDRunoPlugin_TemplateForm::Setup() {
	caption("SDRuno Cosmo Plugin");

	title_bar_label.caption("Cosmo Plugin");
	title_bar_label.move(nana::point(10, 8));
	title_bar_label.size(nana::size(160, 18));

	versionLbl.caption("v1.0");
	versionLbl.text_align(nana::align::right, nana::align_v::center);

	rcLabel.caption("RC: --");
	inrLabel.caption("INR: --");
	lfLabel.caption("LF: --");
	rdeLabel.caption("RDE: --");
	msgLabel.caption("");

	modeCombo.push_back("Modo Restrictivo");
	modeCombo.push_back("Modo Funcional-Libre");
	modeCombo.option(0);

	modeCombo.events().selected([this](const nana::arg_combox&) {
		bool restrictivo = (modeCombo.option() == 0);
		m_parent.SetModeRestrictivo(restrictivo);
	});

	// Configure settings button
	sett_button.move(nana::point(formWidth - 80, 5));
	sett_button.size(nana::size(40, 15));
	sett_button.bgcolor(nana::colors::dark_gray);
	sett_button.events().click([this] { SettingsButton_Click(); });
	
	// Handle close event to clean up settings dialog
	this->events().unload([this](const nana::arg_unload&) {
		if (m_settingsDialog) {
			m_settingsDialog->close();
			m_settingsDialog.reset();
		}
	});
}

void SDRunoPlugin_TemplateForm::Run() {
	show();
	nana::exec();
}

void SDRunoPlugin_TemplateForm::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(3);
	rcLabel.caption("RC: " + oss.str() + std::to_string(rc));
	inrLabel.caption("INR: " + oss.str() + std::to_string(inr));
	lfLabel.caption("LF: " + oss.str() + std::to_string(lf));
	rdeLabel.caption("RDE: " + oss.str() + std::to_string(rde));
	msgLabel.caption(msg);
	if (modoRestrictivo) {
		msgLabel.bgcolor(nana::colors::white);
		msgLabel.fgcolor(nana::colors::black);
	} else {
		msgLabel.bgcolor(nana::colors::light_yellow);
		msgLabel.fgcolor(nana::colors::dark_blue);
	}
}

void SDRunoPlugin_TemplateForm::SettingsButton_Click() {
	if (!m_settingsDialog) {
		// Create a temporary UI wrapper for the settings dialog
		// Since the original design expected SDRunoPlugin_TemplateUi, we need to adapt
		// For now, we'll pass the controller directly and modify the dialog constructor
		m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(m_controller);
	}
	m_settingsDialog->show();
}

void SDRunoPlugin_TemplateForm::SettingsDialog_Closed() {
	// Reset the dialog pointer when it's closed
	m_settingsDialog.reset();
}
