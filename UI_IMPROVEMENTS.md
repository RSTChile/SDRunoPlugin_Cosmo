# SDRuno Cosmo Plugin - UI Window Management Improvements

## Overview
This document describes the comprehensive UI and window management improvements made to the SDRuno Cosmo Plugin to resolve critical issues with window lifecycle, memory management, and user experience.

## Problems Solved

### 1. Window Lifecycle Issues
- **FIXED**: Main window now opens immediately when plugin loads
- **FIXED**: Settings dialog opens only on demand (via Settings button)
- **FIXED**: Both windows close correctly on "Unload All Plugin" or app shutdown
- **FIXED**: No more blocking behavior in constructors

### 2. Nana Window Management  
- **FIXED**: Single main loop pattern - only one `nana::exec()` call
- **FIXED**: Proper cleanup order prevents memory leaks
- **FIXED**: Smart pointers used throughout for automatic memory management
- **FIXED**: Thread-safe window operations with mutex protection

### 3. User Interface Improvements
- **ADDED**: SDRuno-style dark theme for settings dialog
- **ADDED**: Settings button in main window for easy access
- **IMPROVED**: Professional appearance matching SDRuno styling
- **IMPROVED**: Real-time metrics display without blocking

## Architecture Changes

### Main Plugin Class (SDRunoPlugin_Template)
```cpp
// Before: Direct form instantiation
SDRunoPlugin_TemplateForm m_form;

// After: UI manager pattern  
std::unique_ptr<SDRunoPlugin_TemplateUi> m_ui;
```

### UI Manager (SDRunoPlugin_TemplateUi)
- **New single main loop**: `StartMainLoop()` calls `nana::exec()` once
- **Window lifecycle management**: `ShowMainWindow()` and `ShowSettingsDialog()`
- **Proper cleanup**: Destructor closes windows in correct order
- **Event handling**: Responds to SDRuno events properly

### Main Window (SDRunoPlugin_TemplateForm)
- **Non-blocking constructor**: Removed `Run()` method
- **Settings button**: Added for on-demand settings access
- **Event integration**: Proper unload event handling

### Settings Dialog (SDRunoPlugin_TemplateSettingsDialog)
- **Dark theme**: SDRuno-style colors and professional appearance
- **On-demand creation**: Only created when needed
- **Proper cleanup**: Self-removes from UI manager when closed

## Key Features

### ✅ Single Main Loop Pattern
- Only one `nana::exec()` call in entire application
- All windows integrate with single event loop
- No blocking in constructors or window creation

### ✅ Smart Pointer Memory Management
- `unique_ptr<SDRunoPlugin_TemplateUi>` in main plugin
- `shared_ptr<SDRunoPlugin_TemplateForm>` for main window  
- `shared_ptr<SDRunoPlugin_TemplateSettingsDialog>` for settings
- Automatic cleanup prevents memory leaks

### ✅ Proper Event Handling
- `UnoEvent::ClosingDown` closes all windows correctly
- Window close buttons notify UI manager properly
- Thread-safe operations with mutex protection

### ✅ Professional UI Design
- SDRuno-style dark theme (#2D2D30 background)
- Light gray text (#DCDCDC) for good contrast
- Consistent styling across all windows
- Intuitive user experience

## Window Lifecycle Flow

```
1. Plugin Load
   └── Create UI Manager
       └── Show Main Window (non-blocking)
       └── Start Main Loop (single nana::exec)

2. User Interaction
   ├── Settings Button → Open Settings Dialog
   ├── Close Settings → Close Dialog Only
   └── Mode Changes → Update Plugin Logic

3. Plugin Unload/Shutdown
   └── Close Settings Dialog (if open)
   └── Close Main Window
   └── Call nana::API::exit_all()
   └── Clean Shutdown
```

## Files Modified

| File | Changes |
|------|---------|
| `SDRunoPlugin_Template.h/cpp` | UI manager integration, removed direct form |
| `SDRunoPlugin_TemplateUi.h/cpp` | New window management architecture |
| `SDRunoPlugin_TemplateForm.h/cpp` | Non-blocking form, settings button |
| `SDRunoPlugin_TemplateSettingsDialog.h/cpp` | Dark theme, proper lifecycle |

## Testing

The improvements have been validated through:
- ✅ Compilation testing on Linux with GCC
- ✅ Architecture review for proper patterns
- ✅ Memory management verification
- ✅ Event handling validation

## Metrics Processing Unchanged

**Important**: All existing metrics processing and IQ audio processing logic remains completely unchanged:
- RC, INR, LF, RDE calculations unchanged
- Palimpsesto detection unchanged  
- Mode switching (Restrictivo/Funcional-Libre) unchanged
- CSV logging unchanged
- Stream observer processing unchanged

Only UI and window management were modified.

## Future Enhancements

The new architecture supports easy addition of:
- Configuration options in settings dialog
- Additional UI controls
- Plugin preferences
- Advanced metrics display
- Multiple window support

## Compatibility

- ✅ Compatible with existing SDRuno plugin interface
- ✅ Maintains all existing functionality
- ✅ Works with existing configuration system
- ✅ No breaking changes to plugin behavior