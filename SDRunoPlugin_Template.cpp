#include <sstream>
#include <unoevent.h>
#include <iunoplugincontroller.h>
#include <vector>
#include <chrono>
#include <cmath>
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller)
    : IUnoPlugin(controller), m_form(*this, controller), m_worker(nullptr)
{
    m_channel = 0;
    controller.RegisterStreamObserver(m_channel, this);
}

SDRunoPlugin_Template::~SDRunoPlugin_Template()
{
    m_controller.UnregisterStreamObserver(m_channel, this);
}

void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev)
{
    m_form.HandleEvent(ev);
}

void SDRunoPlugin_Template::StreamObserverProcess(channel_t channel, const Complex* data, int length)
{
    // Calcula RMS sin usar std::norm
    float sum = 0.0f;
    for (int i = 0; i < length; ++i) {
        float re = data[i].real;
        float im = data[i].imag;
        sum += re * re + im * im;
    }
    float rms = std::sqrt(sum / length);
    m_signalPresent = (rms > 0.01f);

    m_form.UpdateLed(m_signalPresent);
}

void SDRunoPlugin_Template::WorkerFunction()
{
    // Worker Function Code Goes Here
}