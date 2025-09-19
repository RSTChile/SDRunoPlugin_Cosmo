# SDRunoPlugin_Cosmo

A professional SDRuno plugin template for the Cosmo project. This plugin provides a foundation for creating custom signal processing applications that integrate with SDRuno software-defined radio environment.

## Features

- **Modern C++ Architecture**: Built using C++17 with clean, maintainable code structure
- **Cross-platform GUI**: Uses Nana GUI library for responsive user interface
- **Thread-safe Processing**: Implements proper multi-threading for real-time signal processing
- **SDRuno Integration**: Full compatibility with SDRuno plugin API
- **Frequency Control**: Interactive frequency management and display
- **Status Monitoring**: Real-time status updates and error handling

## Prerequisites

To build and use this plugin, you need:

- **Windows 10/11** (SDRuno is Windows-only)
- **Visual Studio 2019 or later** with C++ development tools
- **SDRuno software** installed
- **Nana GUI library** (included in dependencies)

## Building the Plugin

### Option 1: Visual Studio (Recommended)

1. Open `SDRunoPlugin_Cosmo.sln` in Visual Studio
2. Select your target platform (x86 or x64) - must match your SDRuno installation
3. Choose Release configuration for optimal performance
4. Build the solution (Ctrl+Shift+B)

The compiled DLL will be output to the appropriate platform directory.

### Option 2: CMake

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Installation

1. Build the plugin using one of the methods above
2. Copy `SDRunoPlugin_Cosmo.dll` to your SDRuno plugins directory:
   - Default location: `C:\Program Files\SDRplay\SDRuno\Plugins\`
   - Or user directory: `%APPDATA%\SDRuno\Plugins\`
3. Restart SDRuno
4. The plugin will appear in the plugins menu

## Usage

1. Start SDRuno
2. Go to **Plugins** menu → **Cosmo Plugin**
3. The plugin window will open with frequency controls and status display
4. Use the **Start** button to begin processing
5. Monitor status and adjust frequency as needed
6. Use **Stop** button to halt processing

## Plugin Architecture

### Core Components

- **SDRunoPlugin_Cosmo**: Main plugin class implementing IUnoPlugin interface
- **SDRunoPlugin_CosmoUi**: User interface management and event handling
- **Threading**: Separate processing thread for real-time operations
- **Event System**: Handles SDRuno events (frequency changes, sample rate changes, etc.)

### Key Interfaces

```cpp
// Main plugin interface
class SDRunoPlugin_Cosmo : public IUnoPlugin, 
                          public IUnoStreamObserver, 
                          public IUnoAudioObserver

// UI management interface  
class SDRunoPlugin_CosmoUi
```

### Plugin Lifecycle

1. **Initialization**: Plugin loaded by SDRuno, UI created
2. **Configuration**: User sets parameters via GUI
3. **Processing**: Real-time signal processing in dedicated thread
4. **Cleanup**: Proper shutdown and resource cleanup

## Development

### Adding New Features

1. **Signal Processing**: Add your algorithms to the processing thread in `SDRunoPlugin_CosmoUi.cpp`
2. **UI Controls**: Extend the interface in `Setup()` method
3. **Event Handling**: Add new event handlers in `HandleEvent()` method
4. **Configuration**: Add settings persistence for user preferences

### Code Structure

```
SDRunoPlugin_Cosmo/
├── SDRunoPlugin_Cosmo.h/cpp      # Main plugin implementation
├── SDRunoPlugin_CosmoUi.h/cpp     # User interface management
├── SDRunoPlugin_Cosmo.vcxproj     # Visual Studio project
├── SDRunoPlugin_Cosmo.sln         # Visual Studio solution
├── CMakeLists.txt                 # CMake build configuration
├── SDRunoPlugin_Cosmo.def         # DLL export definitions
├── SDRunoPlugin_Cosmo.rc          # Windows resources
└── README.md                      # This documentation
```

## API Reference

### Main Plugin Methods

- `GetPluginName()`: Returns plugin display name
- `GetPluginVersion()`: Returns version number
- `HandleEvent()`: Processes SDRuno events
- `ShowUi()/HideUi()`: Manages plugin window visibility

### UI Methods

- `Start()/Stop()`: Control processing state
- `SetFrequency()`: Update frequency display
- `UpdateStatus()`: Refresh status information

## Troubleshooting

### Common Issues

**Plugin not loading:**
- Verify DLL architecture matches SDRuno (x86/x64)
- Check Windows dependencies are installed
- Ensure plugin is in correct directory

**UI not responding:**
- Check for threading issues
- Verify event handlers are properly connected
- Monitor Windows event messages

**Build errors:**
- Confirm Visual Studio version compatibility
- Check include paths for SDRuno headers
- Verify Nana GUI library is properly linked

### Debug Mode

Build in Debug configuration for detailed logging and easier troubleshooting.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes with appropriate tests
4. Submit a pull request with clear description

## License

This project is licensed under the GPL v3 License - see the LICENSE file for details.

## Contact

- **Project**: RSTChile SDRuno Plugin Development
- **Repository**: https://github.com/RSTChile/SDRunoPlugin_Cosmo

## Changelog

### Version 1.0.0
- Initial plugin template implementation
- Basic UI with frequency control
- Threading framework for real-time processing
- Full SDRuno API integration
- Visual Studio and CMake build support