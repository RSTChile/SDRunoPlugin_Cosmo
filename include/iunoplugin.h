#pragma once

// SDRuno Plugin API defines
#define SDRUNO_PLUGIN_API 1
#define UNOPLUGINAPI __declspec(dllexport)
#define UNOPLUGINCALL __stdcall

// Forward declarations
class IUnoPluginController;

// Plugin interface base class
class IUnoPlugin
{
public:
    IUnoPlugin(IUnoPluginController& controller) : m_controller(controller) {}
    virtual ~IUnoPlugin() = default;

    // Plugin information
    virtual const char* GetPluginName() const = 0;
    virtual double GetPluginVersion() const = 0;
    virtual const char* GetPluginVendor() const = 0;
    virtual const char* GetPluginDescription() const = 0;
    virtual const int GetSDRplayPluginApi() const = 0;
    virtual const char* GetPluginId() const = 0;

    // Plugin capabilities
    virtual bool HasForm() const = 0;
    virtual bool CanProcessNotifications() const = 0;
    virtual bool HasMenuHandler() const = 0;
    virtual bool HasInfoDisplay() const = 0;

    // Plugin lifecycle
    virtual void HandleEvent(const class UnoEvent& ev) = 0;
    virtual void ShowUi() = 0;
    virtual void HideUi() = 0;

protected:
    IUnoPluginController& m_controller;
};

// Plugin creation function type
typedef IUnoPlugin* (UNOPLUGINCALL *CreatePluginFunc)(IUnoPluginController& controller);