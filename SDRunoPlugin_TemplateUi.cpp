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
			
			// Exit nana event loop
			nana::API::exit_all();
		});
	}
	
	// Wait for GUI thread to finish
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
		// If main form not ready yet, queue the task
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_guiTasks.push(task);
	}
}

void SDRunoPlugin_TemplateUi::GuiThreadMain()
{
	try {
		// Create main window on GUI thread
		CreateMainWindow();
		
		// Signal that GUI thread is ready
		m_guiRunning = true;
		
		// Process any queued tasks before starting event loop
		{
			std::lock_guard<std::mutex> lock(m_taskMutex);
			while (!m_guiTasks.empty()) {
				auto task = m_guiTasks.front();
				m_guiTasks.pop();
				task();
			}
		}
		
		// Run nana event loop - this blocks until exit_all() is called
		nana::exec();
	}
	catch (...) {
		// Handle any exceptions in GUI thread
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

// Show settings dialog - post to GUI thread
void SDRunoPlugin_TemplateUi::ShowSettingsDialog()
{
	PostToGuiThread([this]() {
		if (!m_settingsDialog && m_mainForm) {
			// Create as nested form anchored to main form (propietaria)
			m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(*this, m_controller, *m_mainForm);
			m_settingsDialog->show();
		} else if (m_settingsDialog) {
			// If already open, bring to front
			m_settingsDialog->show();
		}
	});
}

// Update metrics - post to GUI thread
void SDRunoPlugin_TemplateUi::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo)
{
	PostToGuiThread([this, rc, inr, lf, rde, msg, modoRestrictivo]() {
		if (m_mainForm) {
			m_mainForm->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
		}
	});
}

// Callback when settings dialog is closed
void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
	PostToGuiThread([this]() {
		m_settingsDialog.reset();
	});
}

// Load X position from configuration
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

// Load Y position from configuration
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

// Handle events from SDRuno (including Unload Plugin and Shutdown)
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
			// Safely close UI on shutdown
			StopGuiThread();
			FormClosed();
		}
		break;

	default:
		break;
	}
}

// Handle form closed event
void SDRunoPlugin_TemplateUi::FormClosed()
{
	m_controller.RequestUnload(&m_parent);
}
