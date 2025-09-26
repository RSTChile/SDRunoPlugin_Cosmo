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
    
    // Registrar como observador de stream en el canal 0 (canal principal)
    try {
        controller.RegisterStreamObserver(m_channel, this);
    }
    catch (...) {
        // Si falla el registro, podrían haber problemas de captura de señal
        // pero continuamos para evitar crash del plugin
    }
}

SDRunoPlugin_Template::~SDRunoPlugin_Template()
{
    // Desregistrar observador de stream de forma segura
    try {
        m_controller.UnregisterStreamObserver(m_channel, this);
    }
    catch (...) {
        // Ignorar errores en el destructor para evitar excepciones durante shutdown
    }
}

void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev)
{
    // Pasar el evento a la UI para manejo
    m_form.HandleEvent(ev);
    
    // Manejar eventos específicos si es necesario
    switch (ev.GetType())
    {
    case UnoEvent::StreamingStarted:
        // El streaming ha comenzado - asegurarse de que estamos listos para recibir datos
        break;
        
    case UnoEvent::StreamingStopped:
        // El streaming se ha detenido - marcar que no hay señal
        m_signalPresent = false;
        m_form.UpdateLed(false);
        break;
        
    case UnoEvent::VFOSignalPresent:
        // SDRuno detectó señal en el VFO
        break;
        
    case UnoEvent::VFOSignalNotPresent:
        // SDRuno perdió la señal en el VFO
        break;
        
    default:
        // Otros eventos se manejan en la UI
        break;
    }
}

void SDRunoPlugin_Template::StreamObserverProcess(channel_t channel, const Complex* data, int length)
{
    if (!data || length <= 0) {
        return;
    }
    
    // Calcula RMS de la señal compleja
    // Nota: Complex es una struct C con miembros .real e .imag
    float sum = 0.0f;
    for (int i = 0; i < length; ++i) {
        float re = data[i].real;
        float im = data[i].imag;
        sum += re * re + im * im;
    }
    
    float rms = std::sqrt(sum / length);
    
    // Usar un umbral más sensible para detección de señal
    // El umbral de 0.001f detecta señales más débiles
    bool signalDetected = (rms > 0.001f);
    m_signalPresent = signalDetected;

    // Actualizar LED con el estado de la señal
    m_form.UpdateLed(signalDetected);
}

void SDRunoPlugin_Template::WorkerFunction()
{
    // Worker Function Code Goes Here
}