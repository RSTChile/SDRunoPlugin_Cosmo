#pragma once
#include <memory>
#include <atomic>
#include <thread>
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/timer.hpp>

class SDRunoPlugin_CosmoUi
{
public:
    SDRunoPlugin_CosmoUi(IUnoPluginController& controller, IUnoPlugin& plugin);
    ~SDRunoPlugin_CosmoUi();

    void Show();
    void Hide();

private:
    // Core
    IUnoPluginController& m_controller;
    IUnoPlugin& m_plugin;

    // UI
    std::shared_ptr<nana::form>   m_form;
    std::shared_ptr<nana::label>  m_frequencyLabel;
    std::shared_ptr<nana::textbox> m_frequencyTextbox;
    std::shared_ptr<nana::label>  m_statusLabel;

    std::shared_ptr<nana::label>   m_gainLabel;
    std::shared_ptr<nana::textbox> m_gainTextbox;
    std::shared_ptr<nana::button>  m_startButton;
    std::shared_ptr<nana::button>  m_stopButton;
    std::shared_ptr<nana::button>  m_resetButton;
    std::shared_ptr<nana::label>   m_samplesLabel;

    // Worker/thread state
    std::thread m_thread;
    std::atomic<bool> m_started{false};
    std::atomic<long long> m_sampleCount{0};

    // UI refresh timer (runs on GUI thread)
    nana::timer m_uiTimer;

    // Setup & lifecycle
    void Setup();
    void Start();
    void Stop();

    // UI helpers (solo se llaman desde el hilo de GUI)
    void UpdateStatus(const std::string& text);
    void UpdateSampleCountLabel(long long samples);

    // Event handlers (hilo GUI)
    void OnStartClicked();
    void OnStopClicked();
    void OnFrequencyChanged();
    void OnGainChanged();
    void OnResetClicked();
};
