#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunostreamobserver.h>
#include <iunoaudioobserver.h>
#include <iunoaudioprocessor.h>
#include <iunoannotator.h>

// Forward declaration
class SDRunoPlugin_TemplateUi;

class SDRunoPlugin_Template : public IUnoPlugin, public IUnoStreamObserver
{
public:
    SDRunoPlugin_Template(IUnoPluginController& controller);
    virtual ~SDRunoPlugin_Template();

    virtual const char* GetPluginName() const override { return "SDRuno Plugin Cosmo"; }
    virtual void HandleEvent(const UnoEvent& ev) override;

    // Correct implementation of IUnoStreamObserver
    virtual void StreamObserverProcess(channel_t channel, const Complex* data, int length) override;

private:
    void WorkerFunction();
    
    std::thread* m_worker;
    std::mutex m_lock;
    std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;
    std::atomic<bool> m_signalPresent{false};
    channel_t m_channel{0}; // Observed channel
};
