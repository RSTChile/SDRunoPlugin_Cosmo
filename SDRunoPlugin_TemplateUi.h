#pragma once

#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>
#include <string>
#include "iunoplugincontroller.h"

class SDRunoPlugin_Template;
class SDRunoPlugin_TemplateForm;
class SDRunoPlugin_TemplateSettingsDialog;
class UnoEvent;

// UI manager
class SDRunoPlugin_TemplateUi {
public:
    SDRunoPlugin_TemplateUi(SDRunoPlugin_Template& parent, IUnoPluginController& controller);
    ~SDRunoPlugin_TemplateUi();

    void ShowSettingsDialog();
    void UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo);
    void UpdateSavePath(const std::string& path);
    void SetStreamingState(bool streaming);

    // Acciones desde diálogos/GUI
    void ToggleCapture(bool enabled);
    void RequestChangeBaseDir(const std::string& path);
    void RequestChangeVrx(int vrxIndex);

    // Consultas para GUI
    std::string GetBaseDir() const;

    int LoadX();
    int LoadY();
    void HandleEvent(const UnoEvent& ev);
    void FormClosed();
    void SettingsDialogClosed();

    // ...
    void UpdateLed(bool signalPresent);
private:
    SDRunoPlugin_Template& m_parent;
    std::shared_ptr<SDRunoPlugin_TemplateForm> m_mainForm;
    std::shared_ptr<SDRunoPlugin_TemplateSettingsDialog> m_settingsDialog;
    IUnoPluginController& m_controller;

    // Hilo GUI
    std::thread m_guiThread;
    std::atomic<bool> m_guiRunning{false};
    std::atomic<bool> m_shutdownRequested{false};
    std::atomic<bool> m_unloadRequested{false};
    std::mutex m_taskMutex;
    std::queue<std::function<void()>> m_guiTasks;

    void StartGuiThread();
    void StopGuiThread();
    void PostToGuiThread(std::function<void()> task);
    void GuiThreadMain();
    void CreateMainWindow();
};

