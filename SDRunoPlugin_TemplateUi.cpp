#include <sstream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"

// Ui constructor - carga la ventana principal inmediatamente
SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller) :
	m_parent(parent),
	m_mainForm(nullptr),
	m_settingsDialog(nullptr),
	m_controller(controller)
{
	ShowMainWindow();
	StartMainLoop();
}

// Ui destructor: cierra todas las ventanas Nana de forma segura
SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{
	std::lock_guard<std::mutex> guard(m_lock);
	
	// Close settings dialog first if open
	if (m_settingsDialog) {
		m_settingsDialog->close();
		m_settingsDialog.reset();
	}
	
	// Close main form
	if (m_mainForm) {
		m_mainForm->close();
		m_mainForm.reset();
	}
	
	// Only call exit_all after all windows are properly closed
	nana::API::exit_all();
}

// Mostrar la ventana principal (sin bloquear con exec)
void SDRunoPlugin_TemplateUi::ShowMainWindow()
{
	std::lock_guard<std::mutex> guard(m_lock);
	if (!m_mainForm) {
		m_mainForm = std::make_shared<SDRunoPlugin_TemplateForm>(m_parent, m_controller, *this);
		m_mainForm->show();
	}
}

// Mostrar el diálogo de configuración bajo demanda
void SDRunoPlugin_TemplateUi::ShowSettingsDialog()
{
	std::lock_guard<std::mutex> guard(m_lock);
	if (!m_settingsDialog) {
		m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(*this, m_controller);
		m_settingsDialog->show();
	} else {
		// If already open, bring to front
		m_settingsDialog->show();
	}
}

// Iniciar el bucle principal de Nana (solo una vez)
void SDRunoPlugin_TemplateUi::StartMainLoop()
{
	nana::exec();
}

// Actualizar métricas en la ventana principal
void SDRunoPlugin_TemplateUi::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo)
{
	std::lock_guard<std::mutex> guard(m_lock);
	if (m_mainForm) {
		m_mainForm->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
	}
}

// Callback cuando se cierra el diálogo de configuración
void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
	std::lock_guard<std::mutex> guard(m_lock);
	m_settingsDialog.reset();
}

// Update metrics in the form
void SDRunoPlugin_TemplateUi::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo)
{
	std::lock_guard<std::mutex> guard(m_lock);
	if (m_form) {
		m_form->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
	}
}

// Leer X de la ini (si existe)
int SDRunoPlugin_TemplateUi::LoadX()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Template.X", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	return stoi(tmp);
}

// Leer Y de la ini (si existe)
int SDRunoPlugin_TemplateUi::LoadY()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Template.Y", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	return stoi(tmp);
}

// Manejo de eventos desde SDRuno (incluye Unload Plugin y Shutdown)
void SDRunoPlugin_TemplateUi::HandleEvent(const UnoEvent& ev)
{
	switch (ev.GetType())
	{
	case UnoEvent::StreamingStarted:
		break;

	case UnoEvent::StreamingStopped:
		break;

	case UnoEvent::SavingWorkspace:
		break;

	case UnoEvent::ClosingDown:
		{
			std::lock_guard<std::mutex> guard(m_lock);
			
			// Close settings dialog first if open
			if (m_settingsDialog) {
				m_settingsDialog->close();
				m_settingsDialog.reset();
			}
			
			// Close main form
			if (m_mainForm) {
				m_mainForm->close();
				m_mainForm.reset();
			}
			
			// Exit Nana main loop
			nana::API::exit_all();
			FormClosed();
		}
		break;

	default:
		break;
	}
}

// Asegura que el plugin se descarga correctamente cuando se cierra
void SDRunoPlugin_TemplateUi::FormClosed()
{
	m_controller.RequestUnload(&m_parent);
}
