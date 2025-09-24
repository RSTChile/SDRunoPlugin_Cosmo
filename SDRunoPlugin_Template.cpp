#include <sstream>
#include <unoevent.h>
#include <iunoplugincontroller.h>
#include <vector>
#include <chrono>
#include <cmath> // Para sqrt

#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller)
    : IUnoPlugin(controller), m_form(*this, controller), m_worker(nullptr)
{
    controller.RegisterStreamObserver(this);
}

SDRunoPlugin_Template::~SDRunoPlugin_Template()
{
    m_controller.UnregisterStreamObserver(this);
}

void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev)
{
    m_form.HandleEvent(ev);
}

void SDRunoPlugin_Template::StreamUpdate(float* samples, int length)
{
    // Detecta si hay señal (RMS simple)
    float sum = 0.0f;
    for (int i = 0; i < length; ++i) sum += samples[i] * samples[i];
    float rms = sqrt(sum / length);
    m_signalPresent = (rms > 0.01f); // Umbral ajustable

    // Actualiza la UI
    m_form.UpdateLed(m_signalPresent);
}

void SDRunoPlugin_Template::WorkerFunction()
{
    // Worker Function Code Goes Here
}
