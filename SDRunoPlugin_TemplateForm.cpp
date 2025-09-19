#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"
#include <io.h>
#include <shlobj.h>

#define VERSION "V1.1"

// Form constructor con handles a parent y controller - lanza Setup
SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller) :
	nana::form(nana::API::make_center(formWidth, formHeight), nana::appearance(false, true, false, false, true, false, false)),
	m_parent(parent),
	m_controller(controller)
{
	Setup();
}

// Form destructor
SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm()
{

}

// Start Form y comienza Nana UI
void SDRunoPlugin_TemplateForm::Run()
{
	show();
	nana::exec();
}

// Crear el formulario inicial del plugin
void SDRunoPlugin_TemplateForm::Setup()
{
	// --- Carga robusta de recursos embebidos (bitmaps) ---
	HMODULE hModule = nullptr;
	GetModuleHandleExW(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCWSTR>(&SDRunoPlugin_TemplateForm::Setup),
		&hModule
	);

	// Aquí sigue el código del template para cargar los bitmaps, igual que antes pero usando hModule.
	// ... (todo el código de carga de recursos y bitmaps igual que tenías, pero usando hModule como arriba)

	// --- Título y aspecto del panel ---
	caption("SDRuno Plugin Cosmo"); // Cambiado para mostrar "Cosmo"
	events().destroy([&] { m_parent.FormClosed(); });

	//Initialize header bar
	header_bar.size(nana::size(122, 20));
	header_bar.load(img_header, nana::rectangle(0, 0, 122, 20));
	header_bar.stretchable(0, 0, 0, 0);
	header_bar.move(nana::point((formWidth / 2) - 61, 5));
	header_bar.transparent(true);

	//Initial header text 
	title_bar_label.size(nana::size(65, 12));
	title_bar_label.move(nana::point((formWidth / 2) - 5, 9));
	title_bar_label.format(true);
	title_bar_label.caption("< bold size = 6 color = 0x000000 font = \"Verdana\">COSMO</>"); // Cambiado a "COSMO"
	title_bar_label.text_align(nana::align::center, nana::align_v::center);

	// ... (resto del Setup igual, botones, versión, etc.)
}
