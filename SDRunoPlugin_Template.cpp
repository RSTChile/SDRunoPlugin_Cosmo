#include <sstream>
#include <unoevent.h>
#include <iunoplugincontroller.h>
#include <vector>
#include <chrono>
#include <cmath>
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller)
    : IUnoPlugin(controller), m_ui(nullptr), m_worker(nullptr)
{
    m_channel = 0;
    controller.RegisterStreamObserver(m_channel, this);
    
    // Create the UI manager
    m_ui = std::make_unique<SDRunoPlugin_TemplateUi>(*this, controller);
}

SDRunoPlugin_Template::~SDRunoPlugin_Template()
{
    m_controller.UnregisterStreamObserver(m_channel, this);
    
    // Clean up UI first
    m_ui.reset();
    
    // Clean up worker thread if needed
    if (m_worker) {
        if (m_worker->joinable()) {
            m_worker->join();
        }
        delete m_worker;
        m_worker = nullptr;
    }
}

void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev)
{
    if (m_ui) {
        m_ui->HandleEvent(ev);
    }
}

void SDRunoPlugin_Template::StreamObserverProcess(channel_t channel, const Complex* data, int length)
{
    if (!data || length <= 0) return;

    // Calculate RMS - Complex is a C struct with .real and .imag members
    float sum = 0.0f;
    for (int i = 0; i < length; ++i) {
        float re = data[i].real;
        float im = data[i].imag;
        sum += re * re + im * im;
    }
    float rms = std::sqrt(sum / length);
    m_signalPresent = (rms > 0.01f);

    // Update UI with LED state
    if (m_ui) {
        m_ui->UpdateLed(m_signalPresent);
    }
}

void SDRunoPlugin_Template::WorkerFunction()
{
    // Worker Function Code Goes Here
    // This can be used for background processing tasks
}