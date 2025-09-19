# SDRuno Plugin Development Setup

## Quick Start Guide

### Prerequisites Check
- [ ] Windows 10/11 installed
- [ ] Visual Studio 2019+ with C++ tools
- [ ] SDRuno software installed

### Building Your First Plugin

1. **Clone and Setup**
   ```bash
   git clone https://github.com/RSTChile/SDRunoPlugin_Cosmo.git
   cd SDRunoPlugin_Cosmo
   ```

2. **Build Plugin**
   ```cmd
   # Windows Command Prompt
   build.bat
   
   # Or use Visual Studio
   # Open SDRunoPlugin_Cosmo.sln
   # Build -> Build Solution
   ```

3. **Install Plugin**
   - Copy `x64\Release\SDRunoPlugin_Cosmo.dll` to SDRuno plugins folder
   - Default: `C:\Program Files\SDRplay\SDRuno\Plugins\`
   - Restart SDRuno

4. **Test Plugin**
   - Start SDRuno
   - Go to Plugins menu → Cosmo Plugin
   - Verify plugin window opens

### Development Workflow

1. **Make Changes**: Edit source files in your preferred editor
2. **Build**: Run `build.bat` or use Visual Studio
3. **Test**: Copy DLL to plugins folder and test in SDRuno
4. **Debug**: Use Visual Studio debugger if needed

### Common Development Tasks

**Adding new UI controls:**
- Edit `SDRunoPlugin_CosmoUi.cpp` in the `Setup()` method
- Add event handlers for new controls

**Implementing signal processing:**
- Add your algorithms to the processing thread
- Use `ProcessStreamData()` for IQ data
- Use `ProcessAudioData()` for audio

**Handling SDRuno events:**
- Override `HandleEvent()` method
- Respond to frequency/rate changes

### Troubleshooting

**Build Issues:**
- Verify Visual Studio installation
- Check include paths in project settings
- Ensure C++17 standard is selected

**Runtime Issues:**
- Check Windows Event Viewer for errors
- Use Debug build for detailed logging
- Verify plugin architecture matches SDRuno

### Plugin Structure
```
SDRunoPlugin_Cosmo/
├── include/           # SDRuno API headers
├── Source Files:
│   ├── SDRunoPlugin_Cosmo.cpp     # Main plugin
│   └── SDRunoPlugin_CosmoUi.cpp   # User interface
├── Header Files:
│   ├── SDRunoPlugin_Cosmo.h       # Plugin declarations
│   └── SDRunoPlugin_CosmoUi.h     # UI declarations
└── Build Files:
    ├── SDRunoPlugin_Cosmo.sln     # Visual Studio solution
    ├── SDRunoPlugin_Cosmo.vcxproj # Project file
    └── build.bat                  # Build script
```