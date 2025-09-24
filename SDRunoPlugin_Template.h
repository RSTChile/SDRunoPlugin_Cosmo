#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunostreamobserver.h>
#include <iunoaudioobserver.h>
#include <iunoaudioprocessor.h>
#include <iunoannotator.h>

#include "SDRunoPlugin_TemplateUi.h"

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoStreamObserver
{
public:
    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    virtual const char* GetPluginName() const override { return "SDRuno Plugin Example"; }
    virtual void HandleEvent(const UnoEvent& ev) override;

    // Nuevo: Implementación de IUnoStreamObserver
    virtual void StreamUpdate(float* samples, int length) override;

private:
    void WorkerFunction();
    std::thread* m_worker;
    std::mutex m_lock;
    SDRunoPlugin_TemplateUi m_form;
    std::atomic<bool> m_signalPresent{false}; // Estado de señal
};
