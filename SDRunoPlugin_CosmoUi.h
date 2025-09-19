#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <thread>
#include <mutex>

#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunostreamobserver.h>
#include <iunoaudioobserver.h>
#include <iunoaudioprocessor.h>

class SDRunoPlugin_CosmoUi
{

public:

	SDRunoPlugin_CosmoUi(IUnoPluginController& controller, IUnoAudioProcessor& processor);
	~SDRunoPlugin_CosmoUi();

	void HandleEvent(const UnoEvent& ev);
	void FormClosed();
	void ShowUi();
	void HideUi();
	bool IsUiVisible();
	void SetFrequency(long long frequency);

private:

	bool m_started;
	std::thread m_thread;
	std::shared_ptr<nana::form> m_form;
	
	IUnoPluginController& m_controller;
	IUnoAudioProcessor& m_processor;

	std::mutex m_lock;

	// UI Controls
	std::shared_ptr<nana::button> m_startButton;
	std::shared_ptr<nana::button> m_stopButton;
	std::shared_ptr<nana::label> m_frequencyLabel;
	std::shared_ptr<nana::textbox> m_frequencyTextbox;
	std::shared_ptr<nana::label> m_statusLabel;

	void Setup();
	void Start();
	void Stop();
	void UpdateStatus(const std::string& status);
	void OnStartClicked();
	void OnStopClicked();
	void OnFrequencyChanged();
};