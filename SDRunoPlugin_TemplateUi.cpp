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
			try {
				if (m_settingsDialog) {
					m_settingsDialog->close();
					m_settingsDialog.reset();
				}
				if (m_mainForm) {
					m_mainForm->close();
					m_mainForm.reset();
				}
			} catch (...) {
				// swallow
			}
		});
	}

	if (m_guiThread.joinable()) {
		m_guiThread.join();
	}
}

void SDRunoPlugin_TemplateUi::PostToGuiThread(std::function<void()> task)
{
	if (m_shutdownRequested || !m_guiRunning) return;
	
	if (m_mainForm) {
		nana::API::dev::affinity_execute(m_mainForm->handle(), [t = std::move(task)]() {
			try { t(); } catch (...) {}
		});
	} else {
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_guiTasks.push(std::move(task));
	}
}

void SDRunoPlugin_TemplateUi::GuiThreadMain()
{
	try {
		CreateMainWindow();
		m_guiRunning = true;

		{
			std::lock_guard<std::mutex> lock(m_taskMutex);
			while (!m_guiTasks.empty()) {
				auto task = m_guiTasks.front();
				m_guiTasks.pop();
				try { task(); } catch (...) {}
			}
		}

		nana::exec();
	}
	catch (...) {
		// swallow
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

void SDRunoPlugin_TemplateUi::ShowSettingsDialog()
{
	PostToGuiThread([this]() {
		if (!m_settingsDialog && m_mainForm) {
			m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(*this, m_controller, *m_mainForm);
			m_settingsDialog->show();
		} else if (m_settingsDialog) {
			m_settingsDialog->show();
		}
	});
}

void SDRunoPlugin_TemplateUi::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo)
{
	PostToGuiThread([this, rc, inr, lf, rde, msg, modoRestrictivo]() {
		if (m_mainForm) {
			m_mainForm->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
		}
	});
}

void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
	PostToGuiThread([this]() {
		m_settingsDialog.reset();
	});
}

int SDRunoPlugin_TemplateUi::LoadX()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Template.X", tmp);
	if (tmp.empty()) { return -1; }
	try { return stoi(tmp); } catch (...) { return -1; }
}

int SDRunoPlugin_TemplateUi::LoadY()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Template.Y", tmp);
	if (tmp.empty()) { return -1; }
	try { return stoi(tmp); } catch (...) { return -1; }
}

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
		// cerrar UI de forma segura; pedir unload una sola vez
		StopGuiThread();
		FormClosed();
	}
	break;
	default:
		break;
	}
}

void SDRunoPlugin_TemplateUi::FormClosed()
{
	// Evitar reentrancias: pedir unload una sola vez
	bool expected = false;
	if (m_unloadRequested.compare_exchange_strong(expected, true)) {
		m_controller.RequestUnload(&m_parent);
	}
}
