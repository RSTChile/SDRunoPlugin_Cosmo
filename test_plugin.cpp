#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "include/iunoplugincontroller.h"
#include "include/iunoplugin.h"
#include "SDRunoPlugin_Cosmo.h"

// Mock implementation of IUnoPluginController for testing
class MockUnoPluginController : public IUnoPluginController
{
private:
    long long m_frequency = 100000000; // 100 MHz
    int m_sampleRate = 2000000; // 2 MSps
    bool m_streaming = false;

public:
    void SetFrequency(long long frequency) override
    {
        m_frequency = frequency;
        std::cout << "Frequency set to: " << frequency << " Hz" << std::endl;
    }

    long long GetFrequency() const override
    {
        return m_frequency;
    }

    void SetSampleRate(int sampleRate) override
    {
        m_sampleRate = sampleRate;
        std::cout << "Sample rate set to: " << sampleRate << " Sps" << std::endl;
    }

    int GetSampleRate() const override
    {
        return m_sampleRate;
    }

    void StartStreaming() override
    {
        m_streaming = true;
        std::cout << "Streaming started" << std::endl;
    }

    void StopStreaming() override
    {
        m_streaming = false;
        std::cout << "Streaming stopped" << std::endl;
    }

    bool IsStreaming() const override
    {
        return m_streaming;
    }

    void SendEvent(const UnoEvent& event) override
    {
        std::cout << "Event sent: type=" << event.GetType() 
                  << ", freq=" << event.GetFrequency() << std::endl;
    }
};

int main()
{
    std::cout << "SDRuno Cosmo Plugin Test Application" << std::endl;
    std::cout << "====================================" << std::endl;

    // Create mock controller
    auto controller = std::make_unique<MockUnoPluginController>();

    // Create plugin instance
    auto plugin = std::unique_ptr<IUnoPlugin>(CreatePlugin(*controller));

    if (!plugin)
    {
        std::cerr << "Failed to create plugin!" << std::endl;
        return 1;
    }

    // Display plugin information
    std::cout << "Plugin Name: " << plugin->GetPluginName() << std::endl;
    std::cout << "Plugin Version: " << plugin->GetPluginVersion() << std::endl;
    std::cout << "Plugin Vendor: " << plugin->GetPluginVendor() << std::endl;
    std::cout << "Plugin Description: " << plugin->GetPluginDescription() << std::endl;
    std::cout << "Plugin ID: " << plugin->GetPluginId() << std::endl;
    std::cout << "Has Form: " << (plugin->HasForm() ? "Yes" : "No") << std::endl;
    std::cout << "Can Process Notifications: " << (plugin->CanProcessNotifications() ? "Yes" : "No") << std::endl;

    // Test frequency change event
    std::cout << std::endl << "Testing frequency change event..." << std::endl;
    UnoEvent freqEvent(UnoEvent::FrequencyChanged, 145500000); // 145.5 MHz
    plugin->HandleEvent(freqEvent);

    // Test streaming events
    std::cout << std::endl << "Testing streaming events..." << std::endl;
    UnoEvent streamStartEvent(UnoEvent::StreamingStarted);
    plugin->HandleEvent(streamStartEvent);

    UnoEvent streamStopEvent(UnoEvent::StreamingStopped);
    plugin->HandleEvent(streamStopEvent);

    // Note: We can't actually show the UI in a console application
    // In a real SDRuno environment, the UI would be shown by calling plugin->ShowUi()
    std::cout << std::endl << "Plugin test completed successfully!" << std::endl;
    std::cout << "In SDRuno, you would see the plugin UI by going to Plugins -> Cosmo Plugin" << std::endl;

    return 0;
}