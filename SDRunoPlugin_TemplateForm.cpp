#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include <sstream>
#include <iomanip>

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller, SDRunoPlugin_TemplateUi& ui)
	: nana::form(nana::API::make_center(formWidth, formHeight)),
	  m_parent(parent), m_controller(controller), m_ui(ui)
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

	// Settings button setup
	settingsBtn.caption("Settings");
	settingsBtn.events().click([this]() {
		m_ui.ShowSettingsDialog();
	});

	// Handle window close event to properly shut down
	events().unload([this](const nana::arg_unload& arg) {
		m_ui.FormClosed();
	});
}

void SDRunoPlugin_TemplateForm::Run() {
	show();
	nana::exec();
}

void SDRunoPlugin_TemplateForm::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(3);
	
	oss.str(""); oss.clear(); oss << "RC: " << rc;
	rcLabel.caption(oss.str());
	
	oss.str(""); oss.clear(); oss << "INR: " << inr;
	inrLabel.caption(oss.str());
	
	oss.str(""); oss.clear(); oss << "LF: " << lf;
	lfLabel.caption(oss.str());
	
	oss.str(""); oss.clear(); oss << "RDE: " << rde;
	rdeLabel.caption(oss.str());
	
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
		// Create the settings dialog, passing the controller directly.
		m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(m_controller);
	}
	m_settingsDialog->show();
}
