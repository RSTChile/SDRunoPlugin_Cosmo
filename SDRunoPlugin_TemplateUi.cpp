#include <sstream>
#include <chrono>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/programming_interface.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"

// Ui constructor - start GUI thread and create main window
SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller) :
	m_parent(parent),
	m_mainForm(nullptr),
	m_settingsDialog(nullptr),
	m_controller(controller)
{
	StartGuiThread();
}

// Ui destructor: safely stop GUI thread and clean up
SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{
	StopGuiThread();
}

void SDRunoPlugin_TemplateUi::StartGuiThread()
{
	m_guiThread = std::thread([this]() { GuiThreadMain(); });
	
	// Wait for GUI thread to be ready and main window created
	while (!m_guiRunning || !m_mainForm) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void SDRunoPlugin_TemplateUi::StopGuiThread()
{
	if (!m_guiRunning) return;

	m_shutdownRequested = true;

	// Post shutdown task to GUI thread using nana's affinity mechanism
	if (m_mainForm) {
		nana::API::dev::affinity_execute(m_mainForm->handle(), [this]() {
			// Cerrar primero el diálogo de settings si está abierto
			if (m_settingsDialog) {
				m_settingsDialog->close();
				m_settingsDialog.reset();
			}
			// Cerrar la ventana principal
			if (m_mainForm) {
				m_mainForm->close();
				m_mainForm.reset();
			}
			// No usar exit_all(): cerrar nuestras ventanas hace que exec() retorne solo.
		});
	}

	// Esperar a que el hilo GUI termine
	if (m_guiThread.joinable()) {
		m_guiThread.join();
	}
}

void SDRunoPlugin_TemplateUi::PostToGuiThread(std::function<void()> task)
{
	if (m_shutdownRequested || !m_guiRunning) return;
	
	if (m_mainForm) {
		nana::API::dev::affinity_execute(m_mainForm->handle(), task);
	} else {
		// Si la main form aún no está lista, encolar la tarea
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_guiTasks.push(task);
	}
}

void SDRunoPlugin_TemplateUi::GuiThreadMain()
{
	try {
		// Crear ventana principal en hilo GUI
		CreateMainWindow();
		
		// Señalizar que el hilo GUI está listo
		m_guiRunning = true;
		
		// Procesar tareas encoladas antes del event loop
		{
			std::lock_guard<std::mutex> lock(m_taskMutex);
			while (!m_guiTasks.empty()) {
				auto task = m_guiTasks.front();
				m_guiTasks.pop();
				task();
			}
		}
		
		// Ejecutar el bucle de eventos de Nana
		nana::exec();
	}
	catch (...) {
		// Evitar propagar excepciones al host
	}
	
	m_guiRunning = false;
}

void SDRunoPlugin_TemplateUi::CreateMainWindow()
{
	if (!m_mainForm) {
		m_mainForm = std::make_shared<SDRunoPlugin_TemplateForm>(m_parent, m_controller, *this);
		m_mainForm->show();
	}
}

// Mostrar diálogo de settings - post a hilo GUI
void SDRunoPlugin_TemplateUi::ShowSettingsDialog()
{
	PostToGuiThread([this]() {
		if (!m_settingsDialog && m_mainForm) {
			// Owner = ventana principal
			m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(*this, m_controller, *m_mainForm);
			m_settingsDialog->show();
		} else if (m_settingsDialog) {
			// Traer al frente si ya está abierto
			m_settingsDialog->show();
		}
	});
}

// Actualizar métricas - post a hilo GUI
void SDRunoPlugin_TemplateUi::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo)
{
	PostToGuiThread([this, rc, inr, lf, rde, msg, modoRestrictivo]() {
		if (m_mainForm) {
			m_mainForm->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
		}
	});
}

// Callback cuando se cierra el diálogo de settings
void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
	PostToGuiThread([this]() {
		m_settingsDialog.reset();
	});
}

// Cargar X desde configuración
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

// Cargar Y desde configuración
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

// Eventos de SDRuno (Unload/Shutdown)
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
		// Cerrar UI de forma segura en shutdown
		StopGuiThread();
		FormClosed();
	}
	break;

	default:
		break;
	}
}

// Cuando se cierra la forma principal
void SDRunoPlugin_TemplateUi::FormClosed()
{
	m_controller.RequestUnload(&m_parent);
}
