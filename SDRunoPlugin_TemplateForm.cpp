#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_Template.h"

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
	versionLbl.text_align(nana::align::right, nana::align::center);

	rcLabel.caption("RC: --");
	inrLabel.caption("INR: --");
	lfLabel.caption("LF: --");
	rdeLabel.caption("RDE: --");
	msgLabel.caption("");

	modeCombo.push_back("Modo Restrictivo");
	modeCombo.push_back("Modo Funcional-Libre");
	modeCombo.option(0);

	modeCombo.events().selected([this](const nana::arg_combox& arg) {
		bool restrictivo = (modeCombo.option() == 0);
		m_parent.SetModeRestrictivo(restrictivo);
	});

	// Panel y botones ya están en el layout, puedes personalizar imágenes y acciones aquí
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
