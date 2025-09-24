#include <sstream>
#include <thread>
#include <mutex>
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

SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller) :
    m_parent(parent),
    m_form(nullptr),
    m_controller(controller)
{
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
    {
        return -1;
    }
    return stoi(tmp);
}

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
    m_controller.RequestUnload(&m_parent);
}

void SDRunoPlugin_TemplateUi::UpdateLed(bool signalPresent)
{
    std::lock_guard<std::mutex> guard(m_lock);
    if (m_form) m_form->SetLedState(signalPresent);
}

std::string SDRunoPlugin_TemplateUi::GetBaseDir()
{
    std::string baseDir;
    m_controller.GetConfigurationKey("Template.BaseDir", baseDir);
    if (baseDir.empty()) {
        baseDir = "C:\\"; // Directorio por defecto
    }
    return baseDir;
}

void SDRunoPlugin_TemplateUi::RequestChangeVrx(int vrxIndex)
{
    // Stub funcional - cambiar al VRX especificado
    try {
        m_controller.SetVRXEnable(vrxIndex, true);
        // Aquí podrías agregar lógica adicional para cambiar al VRX
    } catch (...) {
        // Manejar errores silenciosamente
    }
}

void SDRunoPlugin_TemplateUi::RequestChangeBaseDir(const std::string& path)
{
    // Stub funcional - guardar el directorio base
    try {
        m_controller.SetConfigurationKey("Template.BaseDir", path);
    } catch (...) {
        // Manejar errores silenciosamente
    }
}

void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
    // Stub funcional - manejar el cierre del diálogo de configuración
    // No necesita hacer nada específico por ahora
}
