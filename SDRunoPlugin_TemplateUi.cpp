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

SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent,
                                                 IUnoPluginController& controller)
    : m_parent(parent),
      m_thread(),
      m_form(nullptr),
      m_controller(controller),
      m_started(false)
{
    // Inicializar carpeta base leyendo la configuración o tomando "." como predeterminado
    std::string tmp;
    m_controller.GetConfigurationKey("Template.BaseDir", tmp);
    m_baseDir = tmp.empty() ? std::string(".") : tmp;

    // Lanzar el hilo de UI
    m_thread = std::thread(&SDRunoPlugin_TemplateUi::ShowUi, this);
}

SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{
    if (m_form) m_form->close();
    if (m_thread.joinable()) m_thread.join();
    nana::API::exit_all();
}

void SDRunoPlugin_TemplateUi::ShowUi()
{
    std::lock_guard<std::mutex> guard(m_lock);
    m_form = std::make_shared<SDRunoPlugin_TemplateForm>(*this, m_controller);
    m_form->Run();
}

int SDRunoPlugin_TemplateUi::LoadX()
{
    std::string tmp;
    m_controller.GetConfigurationKey("Template.X", tmp);
    if (tmp.empty())
        return -1;
    return stoi(tmp);
}

int SDRunoPlugin_TemplateUi::LoadY()
{
    std::string tmp;
    m_controller.GetConfigurationKey("Template.Y", tmp);
    if (tmp.empty())
        return -1;
    return stoi(tmp);
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
        FormClosed();
        break;
    default:
        break;
    }
}

void SDRunoPlugin_TemplateUi::FormClosed()
{
    // Solicitar descarga del plugin. m_parent es de tipo SDRunoPlugin_Template (implementa IUnoPlugin)
    m_controller.RequestUnload(&m_parent);
}

void SDRunoPlugin_TemplateUi::UpdateLed(bool signalPresent)
{
    std::lock_guard<std::mutex> guard(m_lock);
    if (m_form) m_form->SetLedState(signalPresent);
}

std::string SDRunoPlugin_TemplateUi::GetBaseDir() const
{
    return m_baseDir;
}

void SDRunoPlugin_TemplateUi::RequestChangeBaseDir(const std::string& path)
{
    m_baseDir = path;
    try {
        m_controller.SetConfigurationKey("Template.BaseDir", path);
    }
    catch (...) {
        // Ignorar fallos al guardar la configuración
    }
}

void SDRunoPlugin_TemplateUi::RequestChangeVrx(int /*vrxIndex*/)
{
    // No se implementa el cambio de VRX en tiempo de ejecución; función vacía para evitar errores de compilación.
}

void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
    if (m_form) {
        m_form->enabled(true);
        m_form->focus();
    }
}