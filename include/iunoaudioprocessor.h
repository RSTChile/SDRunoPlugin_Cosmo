#pragma once

#include <complex>

// Audio processor interface for plugins that process audio
class IUnoAudioProcessor
{
public:
    virtual ~IUnoAudioProcessor() = default;

    // Audio processing capabilities
    virtual bool CanProcessAudio() const { return false; }
    virtual bool CanProcessIQ() const { return false; }

    // Data processing methods (override as needed)
    virtual void ProcessAudio(float* leftChannel, float* rightChannel, int length) { /* Override if processing audio */ }
    virtual void ProcessIQ(std::complex<float>* data, int length) { /* Override if processing IQ data */ }

    // Configuration
    virtual void SetSampleRate(int sampleRate) { /* Override if needed */ }
    virtual void SetBufferSize(int bufferSize) { /* Override if needed */ }
};