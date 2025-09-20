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

// Ui constructor - carga la ventana principal (sin thread)
SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller) :
	m_parent(parent),
	m_form(nullptr),
	m_controller(controller)
{
	ShowUi();
}

// Ui destructor: cierra todas las ventanas Nana de forma segura
SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{
	if (m_form) {
		m_form->close();
	}
	nana::API::exit_all();
}

// Mostrar y ejecutar el formulario principal
void SDRunoPlugin_TemplateUi::ShowUi()
{
	std::lock_guard<std::mutex> guard(m_lock);
	// CORRECCIÓN: pasa la referencia al plugin, no a la UI
	m_form = std::make_shared<SDRunoPlugin_TemplateForm>(m_parent, m_controller);
	m_form->Run();
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
		if (m_form) {
			m_form->close();
		}
		nana::API::exit_all();
		FormClosed();
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
