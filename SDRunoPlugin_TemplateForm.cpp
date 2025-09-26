#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"

#define formWidth 297
#define formHeight 240
#define VERSION "V1.1"

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(
    SDRunoPlugin_TemplateUi& parent,
    IUnoPluginController& controller)
    : nana::form(nana::API::make_center(formWidth, formHeight),
                 nana::appearance(false, true, false, false, true, false, false)),
      m_parent(parent),
      m_controller(controller)
{
    Setup();
}

SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm()
{
    if (m_uiTimer) {
        m_uiTimer->stop();
    }
}

void SDRunoPlugin_TemplateForm::Setup()
{
    // Set up the main window
    caption("SDRuno Plugin - COSMO");
    
    // Load saved position
    int x = m_parent.LoadX();
    int y = m_parent.LoadY();
    if (x >= 0 && y >= 0) {
        move(x, y);
    }
    
    // Title label
    m_titleLabel = std::make_unique<nana::label>(*this, nana::rectangle{10, 10, 250, 25});
    m_titleLabel->caption("SDRuno Plugin COSMO");
    m_titleLabel->text_align(nana::align::center);
    m_titleLabel->typeface(nana::paint::font{"Segoe UI", 12, {nana::paint::font::font_style::bold}});
    
    // Version label
    m_versionLabel = std::make_unique<nana::label>(*this, nana::rectangle{10, 35, 250, 20});
    m_versionLabel->caption(VERSION);
    m_versionLabel->text_align(nana::align::center);
    m_versionLabel->fgcolor(nana::color(128, 128, 128));
    
    // Signal LED panel
    m_signalLed = std::make_unique<nana::panel>(*this, nana::rectangle{20, 70, 20, 20});
    m_signalLed->bgcolor(nana::color(255, 0, 0)); // Red by default (no signal)
    
    // Signal status label
    m_signalLabel = std::make_unique<nana::label>(*this, nana::rectangle{50, 70, 200, 20});
    m_signalLabel->caption("Signal: Waiting...");
    m_signalLabel->fgcolor(nana::color(80, 80, 80));
    
    // Settings button
    m_settingsButton = std::make_unique<nana::button>(*this, nana::rectangle{10, 110, 100, 30});
    m_settingsButton->caption("Settings");
    m_settingsButton->events().click([this]() {
        OnSettingsClick();
    });
    
    // Set up timer for UI updates (100ms interval)
    m_uiTimer = std::make_unique<nana::timer>();
    m_uiTimer->interval(std::chrono::milliseconds(100));
    m_uiTimer->elapse([this]() {
        UpdateSignalLed();
    });
    m_uiTimer->start();
    
    // Handle close event
    events().unload([this](const nana::arg_unload& arg) {
        OnFormClose();
    });
}

void SDRunoPlugin_TemplateForm::Run()
{
    show();
    nana::exec();
}

void SDRunoPlugin_TemplateForm::SetLedState(bool signalPresent)
{
    m_signalPresent = signalPresent;
}

void SDRunoPlugin_TemplateForm::UpdateSignalLed()
{
    std::lock_guard<std::mutex> guard(m_uiMutex);
    
    if (m_signalLed && m_signalLabel) {
        bool hasSignal = m_signalPresent.load();
        
        if (hasSignal) {
            m_signalLed->bgcolor(nana::color(0, 255, 0)); // Green for signal present
            m_signalLabel->caption("Signal: DETECTED");
            m_signalLabel->fgcolor(nana::color(0, 128, 0));
        } else {
            m_signalLed->bgcolor(nana::color(255, 0, 0)); // Red for no signal
            m_signalLabel->caption("Signal: Waiting...");
            m_signalLabel->fgcolor(nana::color(128, 0, 0));
        }
    }
}

int SDRunoPlugin_TemplateForm::GetLoadX()
{
    return static_cast<int>(pos().x);
}

int SDRunoPlugin_TemplateForm::GetLoadY()
{
    return static_cast<int>(pos().y);
}

void SDRunoPlugin_TemplateForm::OnFormClose()
{
    // Save position before closing
    try {
        std::stringstream x_stream, y_stream;
        x_stream << pos().x;
        y_stream << pos().y;
        m_controller.SetConfigurationKey("Template.X", x_stream.str());
        m_controller.SetConfigurationKey("Template.Y", y_stream.str());
    }
    catch (...) {
        // Ignore configuration save errors
    }
    
    // Notify parent that form is closing
    m_parent.FormClosed();
}

void SDRunoPlugin_TemplateForm::OnSettingsClick()
{
    try {
        // Disable main form while settings dialog is open
        enabled(false);
        
        // Create and show settings dialog
        auto settingsDialog = std::make_unique<SDRunoPlugin_TemplateSettingsDialog>(m_parent, *this, m_controller);
        settingsDialog->show();
        
        // The dialog will call SettingsDialogClosed when it's done
    }
    catch (...) {
        // Re-enable form if settings dialog fails
        enabled(true);
        focus();
    }
}