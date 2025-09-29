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

    // Calculate basic signal metrics
    float sum_power = 0.0f;
    float max_power = 0.0f;
    
    for (int i = 0; i < length; ++i) {
        float re = data[i].real;
        float im = data[i].imag;
        float power = re * re + im * im;
        sum_power += power;
        if (power > max_power) {
            max_power = power;
        }
    }
    
    float rms = std::sqrt(sum_power / length);
    m_signalPresent = (rms > 0.01f);
    
    // Calculate basic metrics (placeholder implementations)
    // These should be replaced with proper signal processing algorithms
    float rc = rms * 100.0f; // RC metric placeholder
    float inr = (max_power > 0.0f) ? (rms / std::sqrt(max_power)) * 100.0f : 0.0f; // INR placeholder
    float lf = rms * 50.0f; // LF metric placeholder  
    float rde = (sum_power > 0.0f) ? (max_power / (sum_power / length)) : 0.0f; // RDE placeholder

    // Update UI with metrics and LED state
    if (m_ui) {
        m_ui->UpdateLed(m_signalPresent);
        m_ui->UpdateMetrics(rc, inr, lf, rde);
    }
}

void SDRunoPlugin_Template::WorkerFunction()
{
    // Worker Function Code Goes Here
    // This can be used for background processing tasks
}