#include <sstream>
#include <chrono>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/programming_interface.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"

SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller) :
    m_parent(parent),
    m_mainForm(nullptr),
    m_settingsDialog(nullptr),
    m_controller(controller)
{
    StartGuiThread();
}

SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{
    StopGuiThread();
}

void SDRunoPlugin_TemplateUi::StartGuiThread()
{
    m_guiThread = std::thread([this]() { GuiThreadMain(); });
    // Esperar a que la ventana principal esté lista para postear tareas
    for (int i = 0; i < 500; ++i) {
        if (m_guiRunning.load() && m_mainForm) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void SDRunoPlugin_TemplateUi::StopGuiThread()
{
    if (!m_guiRunning) return;

    m_shutdownRequested = true;

    if (m_settingsDialog) {
        // Cerrar settings si está abierta
        nana::API::dev::affinity_execute(m_settingsDialog->handle(), [dlg = m_settingsDialog]() {
            try { dlg->close(); } catch (...) {}
        });
    }

    if (m_mainForm) {
        // Cerrar explícitamente la ventana; al cerrar la última, nana::exec() retornará
        nana::API::dev::affinity_execute(m_mainForm->handle(), [this]() {
            try { m_mainForm->close(); } catch (...) {}
        });
    }

    if (m_guiThread.joinable()) { m_guiThread.join(); }

    m_settingsDialog.reset();
    m_mainForm.reset();
    m_guiRunning = false;
}

void SDRunoPlugin_TemplateUi::PostToGuiThread(std::function<void()> task)
{
    if (m_shutdownRequested || !m_guiRunning) return;
    if (m_mainForm) {
        nana::API::dev::affinity_execute(m_mainForm->handle(), [t = std::move(task)]() { try { t(); } catch (...) {} });
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
                auto task = m_guiTasks.front(); m_guiTasks.pop();
                try { task(); } catch (...) {}
            }
        }
        nana::exec();
    } catch (...) {}
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
            m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(*this, *m_mainForm, m_controller);
            m_settingsDialog->show();
        } else if (m_settingsDialog) {
            m_settingsDialog->show();
        }
    });
}

void SDRunoPlugin_TemplateUi::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo)
{
    PostToGuiThread([this, rc, inr, lf, rde, msg, modoRestrictivo]() {
        if (m_mainForm) { m_mainForm->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo); }
    });
}

void SDRunoPlugin_TemplateUi::UpdateSavePath(const std::string& path)
{
    PostToGuiThread([this, path]() {
        if (m_mainForm) { m_mainForm->SetSavePath(path); }
    });
}

void SDRunoPlugin_TemplateUi::SetStreamingState(bool streaming)
{
    PostToGuiThread([this, streaming]() {
        if (m_mainForm) { m_mainForm->SetStreaming(streaming); }
    });
}

void SDRunoPlugin_TemplateUi::ToggleCapture(bool enabled)
{
    m_parent.SetCaptureEnabled(enabled);
}

void SDRunoPlugin_TemplateUi::RequestChangeBaseDir(const std::string& path)
{
    m_parent.RequestChangeBaseDirAsync(path);
}

void SDRunoPlugin_TemplateUi::RequestChangeVrx(int vrxIndex)
{
    m_parent.ChangeVrxAsync(vrxIndex);
}

std::string SDRunoPlugin_TemplateUi::GetBaseDir() const
{
    return m_parent.GetBaseDirSafe();
}

void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
    PostToGuiThread([this]() { m_settingsDialog.reset(); });
}

int SDRunoPlugin_TemplateUi::LoadX() { return -1; }
int SDRunoPlugin_TemplateUi::LoadY() { return -1; }

void SDRunoPlugin_TemplateUi::HandleEvent(const UnoEvent& ev)
{
    switch (ev.GetType())
    {
    case UnoEvent::StreamingStarted:
        SetStreamingState(true);
        break;
    case UnoEvent::StreamingStopped:
        SetStreamingState(false);
        break;
    case UnoEvent::SavingWorkspace:
        break;
    case UnoEvent::ClosingDown:
    {
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
    StopGuiThread();

    bool expected = false;
    if (m_unloadRequested.compare_exchange_strong(expected, true)) {
        m_parent.RequestUnloadAsync();
    }
}
