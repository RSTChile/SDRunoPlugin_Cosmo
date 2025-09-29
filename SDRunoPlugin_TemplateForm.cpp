#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "resource.h"
#include <sstream>
#include <iomanip>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#endif

#define VERSION "V1.1"

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(
    SDRunoPlugin_TemplateUi& parent,
    IUnoPluginController& controller)
    : nana::form(nana::API::make_center(FORM_WIDTH, FORM_HEIGHT),
                 nana::appear::decorate<nana::appear::taskbar, nana::appear::minimize, nana::appear::maximize>()),
      m_parent(parent),
      m_controller(controller),
      m_titleLabel(*this, "Cosmo Plugin"),
      m_versionLabel(*this, VERSION),
      m_rcLabel(*this, "RC --"),
      m_inrLabel(*this, "INR --"),
      m_lfLabel(*this, "LF --"),
      m_rdeLabel(*this, "RDE --"),
      m_ledLabel(*this, "LED"),
      m_modeLabel(*this, "Mode: Restrictivo"),
      m_settingsBtn(*this, "Settings"),
      m_unloadBtn(*this, "Unload"),
      m_ledPanel(*this),
      m_signalPresent(false),
      m_restrictiveMode(true),
      m_lastRC(0.0f),
      m_lastINR(0.0f),
      m_lastLF(0.0f),
      m_lastRDE(0.0f)
{
    Setup();
}

SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm()
{
    if (m_timer) {
        m_timer->stop();
        m_timer.reset();
    }
}

void SDRunoPlugin_TemplateForm::Setup()
{
    caption("SDRuno Plugin Cosmo");
    
    // Configure title label
    m_titleLabel.move(nana::rectangle(10, 8, 160, 18));
    
    // Configure version label
    m_versionLabel.move(nana::rectangle(FORM_WIDTH - 60, 8, 50, 18));
    
    // Configure metric labels
    m_rcLabel.move(nana::rectangle(10, 40, 100, 18));
    m_inrLabel.move(nana::rectangle(120, 40, 100, 18));
    m_lfLabel.move(nana::rectangle(10, 65, 100, 18));
    m_rdeLabel.move(nana::rectangle(120, 65, 100, 18));
    
    // Configure LED panel and label
    m_ledPanel.move(nana::rectangle(10, 95, 40, 20));
    m_ledPanel.bgcolor(nana::colors::red);
    m_ledLabel.move(nana::rectangle(55, 98, 50, 18));
    
    // Configure mode label
    m_modeLabel.move(nana::rectangle(10, 125, 180, 18));
    
    // Configure buttons
    m_settingsBtn.move(nana::rectangle(10, FORM_HEIGHT - 60, 80, 25));
    m_unloadBtn.move(nana::rectangle(FORM_WIDTH - 90, FORM_HEIGHT - 60, 80, 25));
    
    // Set up event handlers
    m_settingsBtn.events().click([this]() {
        OnSettingsClick();
    });
    
    m_unloadBtn.events().click([this]() {
        OnUnload();
    });
    
    // Form close event
    events().unload([this](const nana::arg_unload& arg) {
        OnUnload();
    });
    
    // Start metrics timer
    StartMetricsTimer();
}

void SDRunoPlugin_TemplateForm::OnUnload()
{
    if (m_timer) {
        m_timer->stop();
    }
    m_parent.FormClosed();
}

void SDRunoPlugin_TemplateForm::OnSettingsClick()
{
    ShowSettingsDialog();
}

void SDRunoPlugin_TemplateForm::ShowSettingsDialog()
{
    try {
        auto dialog = std::make_unique<SDRunoPlugin_TemplateSettingsDialog>(m_parent, *this, m_controller);
        dialog->show();
        // Dialog will manage its own lifecycle
    } catch (...) {
        // Handle dialog creation errors silently
    }
}

void SDRunoPlugin_TemplateForm::StartMetricsTimer()
{
    m_timer = std::make_unique<nana::timer>();
    m_timer->interval(std::chrono::milliseconds(100)); // Update every 100ms
    m_timer->elapse([this]() {
        UpdateUI();
    });
    m_timer->start();
}

void SDRunoPlugin_TemplateForm::UpdateUI()
{
    // Update LED color based on signal presence
    if (m_signalPresent) {
        m_ledPanel.bgcolor(nana::colors::green);
    } else {
        m_ledPanel.bgcolor(nana::colors::red);
    }
    
    // Update metric labels
    std::stringstream ss;
    ss << "RC " << std::fixed << std::setprecision(2) << m_lastRC;
    m_rcLabel.caption(ss.str());
    
    ss.str("");
    ss << "INR " << std::fixed << std::setprecision(2) << m_lastINR;
    m_inrLabel.caption(ss.str());
    
    ss.str("");
    ss << "LF " << std::fixed << std::setprecision(2) << m_lastLF;
    m_lfLabel.caption(ss.str());
    
    ss.str("");
    ss << "RDE " << std::fixed << std::setprecision(2) << m_lastRDE;
    m_rdeLabel.caption(ss.str());
    
    // Update mode label
    m_modeLabel.caption(m_restrictiveMode ? "Mode: Restrictivo" : "Mode: Funcional-Libre");
}

void SDRunoPlugin_TemplateForm::HandleEvent(const UnoEvent& ev)
{
    switch (ev.GetType())
    {
    case UnoEvent::FrequencyChanged:
    case UnoEvent::CenterFrequencyChanged:
        // Handle frequency change events
        break;
    case UnoEvent::StreamingStarted:
        // Handle streaming started
        break;
    case UnoEvent::StreamingStopped:
        // Handle streaming stopped
        break;
    default:
        break;
    }
}

void SDRunoPlugin_TemplateForm::SetLedState(bool signalPresent)
{
    m_signalPresent = signalPresent;
}

void SDRunoPlugin_TemplateForm::UpdateMetrics(float rc, float inr, float lf, float rde)
{
    m_lastRC = rc;
    m_lastINR = inr;
    m_lastLF = lf;
    m_lastRDE = rde;
}

void SDRunoPlugin_TemplateForm::SetMode(bool restrictive)
{
    m_restrictiveMode = restrictive;
}

int SDRunoPlugin_TemplateForm::GetLoadX()
{
    return 100; // Default X position
}

int SDRunoPlugin_TemplateForm::GetLoadY()
{
    return 100; // Default Y position
}

void SDRunoPlugin_TemplateForm::Run()
{
    // This method is provided for compatibility
    // In the new architecture, the UI manager handles window lifecycle
    show();
}