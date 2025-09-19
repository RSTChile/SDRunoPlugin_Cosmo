#pragma once

// Event types
class UnoEvent
{
public:
    enum EventType
    {
        FrequencyChanged = 1,
        RateChanged = 2,
        StreamingStarted = 3,
        StreamingStopped = 4,
        AudioStarted = 5,
        AudioStopped = 6
    };

    UnoEvent(EventType type) : m_type(type), m_frequency(0), m_sampleRate(0) {}
    UnoEvent(EventType type, long long frequency) : m_type(type), m_frequency(frequency), m_sampleRate(0) {}
    UnoEvent(EventType type, long long frequency, int sampleRate) : m_type(type), m_frequency(frequency), m_sampleRate(sampleRate) {}

    EventType GetType() const { return m_type; }
    long long GetFrequency() const { return m_frequency; }
    int GetSampleRate() const { return m_sampleRate; }

private:
    EventType m_type;
    long long m_frequency;
    int m_sampleRate;
};

// Plugin controller interface
class IUnoPluginController
{
public:
    virtual ~IUnoPluginController() = default;

    // Frequency control
    virtual void SetFrequency(long long frequency) = 0;
    virtual long long GetFrequency() const = 0;

    // Sample rate control
    virtual void SetSampleRate(int sampleRate) = 0;
    virtual int GetSampleRate() const = 0;

    // Streaming control
    virtual void StartStreaming() = 0;
    virtual void StopStreaming() = 0;
    virtual bool IsStreaming() const = 0;

    // Event notifications
    virtual void SendEvent(const UnoEvent& event) = 0;
};