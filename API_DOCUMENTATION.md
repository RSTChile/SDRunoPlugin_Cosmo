# SDRuno Plugin API Documentation

## Overview

This document provides detailed information about the SDRuno Plugin API as implemented in the Cosmo plugin template.

## Plugin Architecture

### Core Classes

#### IUnoPlugin
The main plugin interface that all SDRuno plugins must implement.

```cpp
class IUnoPlugin
{
    // Plugin identification
    virtual const char* GetPluginName() const = 0;
    virtual double GetPluginVersion() const = 0;
    virtual const char* GetPluginVendor() const = 0;
    virtual const char* GetPluginDescription() const = 0;
    virtual const char* GetPluginId() const = 0;
    
    // Plugin capabilities
    virtual bool HasForm() const = 0;
    virtual bool CanProcessNotifications() const = 0;
    
    // Plugin lifecycle
    virtual void HandleEvent(const UnoEvent& ev) = 0;
    virtual void ShowUi() = 0;
    virtual void HideUi() = 0;
};
```

#### IUnoPluginController
Provides access to SDRuno's core functionality.

```cpp
class IUnoPluginController
{
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
};
```

### Observer Interfaces

#### IUnoStreamObserver
For plugins that need to process IQ data streams.

```cpp
class IUnoStreamObserver
{
    virtual void StreamProcessingStarted() = 0;
    virtual void StreamProcessingStopped() = 0;
    virtual void StreamProcessingFlushed() = 0;
    virtual void ProcessStreamData(const std::complex<float>* data, int length);
};
```

#### IUnoAudioObserver
For plugins that need to process demodulated audio.

```cpp
class IUnoAudioObserver
{
    virtual void AudioProcessingStarted() = 0;
    virtual void AudioProcessingStopped() = 0;
    virtual void ProcessAudioData(const float* left, const float* right, int length);
};
```

## Event System

### UnoEvent Types

- **FrequencyChanged**: SDRuno frequency has changed
- **RateChanged**: Sample rate has changed  
- **StreamingStarted**: IQ streaming has started
- **StreamingStopped**: IQ streaming has stopped
- **AudioStarted**: Audio processing has started
- **AudioStopped**: Audio processing has stopped

### Event Handling

```cpp
void SDRunoPlugin_Cosmo::HandleEvent(const UnoEvent& ev)
{
    switch (ev.GetType())
    {
    case UnoEvent::FrequencyChanged:
        // Update plugin with new frequency
        SetFrequency(ev.GetFrequency());
        break;
    case UnoEvent::RateChanged:
        // Update plugin with new sample rate
        break;
    // ... handle other events
    }
}
```

## User Interface

### Nana GUI Framework

The Cosmo plugin uses the Nana C++ GUI library for cross-platform user interface development.

#### Basic UI Setup

```cpp
void SDRunoPlugin_CosmoUi::Setup()
{
    // Create form
    m_form = std::make_shared<nana::form>(nana::API::make_center(450, 200));
    m_form->caption("My Plugin");
    
    // Add controls
    auto button = std::make_shared<nana::button>(*m_form, nana::rectangle(10, 10, 80, 30));
    button->caption("Click Me");
    
    // Event handlers
    button->events().click([this] { OnButtonClick(); });
}
```

#### Common UI Patterns

**Text Input with Validation:**
```cpp
m_textbox->events().text_changed([this] {
    try {
        double value = std::stod(m_textbox->caption());
        // Use validated value
    } catch (...) {
        // Handle invalid input
    }
});
```

**Status Updates:**
```cpp
void UpdateStatus(const std::string& message)
{
    if (m_statusLabel) {
        m_statusLabel->caption("Status: " + message);
    }
}
```

## Threading and Synchronization

### Thread Safety

- Always use mutex protection for shared data
- UI updates must be performed on the main thread
- Signal processing should be done in separate threads

```cpp
class PluginUi
{
    std::mutex m_lock;
    
    void Start() {
        std::lock_guard<std::mutex> lock(m_lock);
        if (!m_started) {
            m_started = true;
            m_thread = std::thread([this] { ProcessingLoop(); });
        }
    }
};
```

### Processing Loop Pattern

```cpp
void ProcessingLoop()
{
    while (m_started)
    {
        // Process data
        ProcessSamples();
        
        // Update UI periodically
        if (ShouldUpdateUI()) {
            UpdateDisplay();
        }
        
        // Prevent excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
```

## Signal Processing

### IQ Data Processing

```cpp
void ProcessStreamData(const std::complex<float>* data, int length)
{
    for (int i = 0; i < length; i++)
    {
        std::complex<float> sample = data[i];
        
        // Apply your signal processing here
        // Example: simple gain
        sample *= m_gain;
        
        // Example: frequency shift
        sample *= std::exp(std::complex<float>(0, m_phaseAccumulator));
        m_phaseAccumulator += m_phaseIncrement;
    }
}
```

### Audio Processing

```cpp
void ProcessAudioData(const float* left, const float* right, int length)
{
    for (int i = 0; i < length; i++)
    {
        float leftSample = left[i];
        float rightSample = right[i];
        
        // Apply audio processing
        // Example: volume control
        leftSample *= m_volume;
        rightSample *= m_volume;
    }
}
```

## Plugin Configuration

### Persistent Settings

```cpp
// Save settings to registry or file
void SaveSettings()
{
    // Implementation depends on chosen storage method
    WriteSettingDouble("gain", m_gain);
    WriteSettingLongLong("frequency", m_frequency);
}

void LoadSettings()
{
    m_gain = ReadSettingDouble("gain", 1.0); // default 1.0
    m_frequency = ReadSettingLongLong("frequency", 100000000); // default 100MHz
}
```

## Build Configuration

### Visual Studio Project Settings

**Required Preprocessor Definitions:**
- `SDRUNOPLUGINCOSMO_EXPORTS`
- `_WINDOWS`
- `_USRDLL`
- `UNICODE`

**Required Libraries:**
- `user32.lib`
- `gdi32.lib`
- `shell32.lib`
- `nana.lib` (GUI framework)

**Module Definition File:**
```
EXPORTS
CreatePlugin
```

### CMake Configuration

```cmake
add_library(MyPlugin SHARED ${SOURCES})
target_compile_definitions(MyPlugin PRIVATE
    MYPLUGIN_EXPORTS
    _WINDOWS
    _USRDLL
)
target_link_libraries(MyPlugin PRIVATE nana user32 gdi32)
```

## Debugging

### Debug Build Configuration

- Enable debug symbols (`/Zi`)
- Disable optimizations (`/Od`)
- Add runtime checks (`/RTC1`)

### Common Debugging Techniques

```cpp
// Debug output
#ifdef _DEBUG
    OutputDebugStringA("Plugin debug message\n");
#endif

// Exception handling
try {
    // Plugin code
} catch (const std::exception& e) {
    std::string error = "Plugin error: " + std::string(e.what());
    UpdateStatus(error);
}
```

## Performance Considerations

### Optimization Tips

1. **Minimize allocations** in processing loops
2. **Use SIMD** instructions for heavy computation
3. **Cache frequently accessed data**
4. **Avoid blocking operations** in audio callbacks
5. **Use appropriate buffer sizes**

### Memory Management

```cpp
// Pre-allocate buffers
class AudioProcessor {
    std::vector<float> m_workBuffer;
    
    AudioProcessor() {
        m_workBuffer.resize(4096); // Typical audio buffer size
    }
    
    void ProcessAudio(const float* input, int length) {
        // Reuse pre-allocated buffer
        if (m_workBuffer.size() < length) {
            m_workBuffer.resize(length);
        }
        // Process without allocation
    }
};
```

## Best Practices

1. **Error Handling**: Always handle exceptions gracefully
2. **Resource Cleanup**: Ensure proper cleanup in destructors
3. **Thread Safety**: Use appropriate synchronization
4. **User Experience**: Provide clear status feedback
5. **Documentation**: Comment complex algorithms
6. **Testing**: Test with various input conditions
7. **Performance**: Profile critical paths

## Example Implementations

See the Cosmo plugin source code for complete examples of:
- Basic plugin structure
- GUI implementation with Nana
- Event handling
- Threading patterns
- Signal processing framework