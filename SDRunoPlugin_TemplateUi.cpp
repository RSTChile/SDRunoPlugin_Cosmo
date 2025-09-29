#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateForm.h"
#include <sstream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <unoevent.h>

SDRunoPlugin_TemplateUi::SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent,
                                                 IUnoPluginController& controller)
    : m_parent(parent),
      m_thread(),
      m_form(nullptr),
      m_controller(controller),
      m_started(false)
{
    // Initialize base directory from configuration or use default
    std::string tmp;
    m_controller.GetConfigurationKey("Template.BaseDir", tmp);
    m_baseDir = tmp.empty() ? std::string(".") : tmp;

    // Launch the UI thread
    m_thread = std::thread(&SDRunoPlugin_TemplateUi::ShowUi, this);
}

SDRunoPlugin_TemplateUi::~SDRunoPlugin_TemplateUi()
{
    {
        std::lock_guard<std::mutex> guard(m_lock);
        if (m_form) {
            m_form->close();
            m_form.reset();
        }
    }
    
    if (m_thread.joinable()) {
        m_thread.join();
    }
    
    // Clean up any remaining Nana resources
    try {
        nana::API::exit_all();
    } catch (...) {
        // Ignore cleanup errors
    }
}

void SDRunoPlugin_TemplateUi::ShowUi()
{
    try {
        std::lock_guard<std::mutex> guard(m_lock);
        if (!m_started) {
            m_started = true;
            
            // Create the main form
            m_form = std::make_shared<SDRunoPlugin_TemplateForm>(*this, m_controller);
            
            // Show the form
            m_form->show();
            
            // Run the message loop
            nana::exec();
        }
    } catch (...) {
        // Handle any UI creation errors
        m_started = false;
    }
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
    {
        std::lock_guard<std::mutex> guard(m_lock);
        if (m_form) {
            m_form->HandleEvent(ev);
        }
    }

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
    // Request plugin unload. m_parent is of type SDRunoPlugin_Template (implements IUnoPlugin)
    m_controller.RequestUnload(&m_parent);
}

void SDRunoPlugin_TemplateUi::UpdateLed(bool signalPresent)
{
    std::lock_guard<std::mutex> guard(m_lock);
    if (m_form) {
        m_form->SetLedState(signalPresent);
    }
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
    } catch (...) {
        // Ignore configuration save failures
    }
}

void SDRunoPlugin_TemplateUi::RequestChangeVrx(int /*vrxIndex*/)
{
    // VRX changing at runtime not implemented; empty function to avoid compilation errors
}

void SDRunoPlugin_TemplateUi::SettingsDialogClosed()
{
    std::lock_guard<std::mutex> guard(m_lock);
    if (m_form) {
        m_form->enabled(true);
        m_form->focus();
    }
}