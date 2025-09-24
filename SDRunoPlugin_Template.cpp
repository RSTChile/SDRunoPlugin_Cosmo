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
    m_channel = 0; // O el canal que corresponda
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
    // Calcula RMS de la señal
    float sum = 0.0f;
    for (int i = 0; i < length; ++i)
        sum += std::norm(data[i]);
    float rms = sqrt(sum / length);
    m_signalPresent = (rms > 0.01f);

    m_form.UpdateLed(m_signalPresent);
}

void SDRunoPlugin_Template::WorkerFunction()
{
    // Worker Function Code Goes Here
}
