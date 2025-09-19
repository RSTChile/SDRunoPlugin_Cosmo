#pragma once

#include <complex>

// Stream observer interface for IQ data
class IUnoStreamObserver
{
public:
    virtual ~IUnoStreamObserver() = default;

    // Stream lifecycle events
    virtual void StreamProcessingStarted() = 0;
    virtual void StreamProcessingStopped() = 0;
    virtual void StreamProcessingFlushed() = 0;

    // IQ data processing (if implemented)
    virtual void ProcessStreamData(const std::complex<float>* data, int length) { /* Optional override */ }
};