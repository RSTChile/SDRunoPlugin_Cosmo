#pragma once

// Audio observer interface for demodulated audio
class IUnoAudioObserver
{
public:
    virtual ~IUnoAudioObserver() = default;

    // Audio lifecycle events
    virtual void AudioProcessingStarted() = 0;
    virtual void AudioProcessingStopped() = 0;

    // Audio data processing (if implemented)
    virtual void ProcessAudioData(const float* leftChannel, const float* rightChannel, int length) { /* Optional override */ }
};