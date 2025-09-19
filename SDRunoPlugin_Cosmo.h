#pragma once

#include <iunoplugin.h>
#include <iunoplugincontroller.h>
#include <iunostreamobserver.h>
#include <iunoaudioobserver.h>
#include <iunoaudioprocessor.h>
#include "SDRunoPlugin_CosmoUi.h"

class SDRunoPlugin_Cosmo : public IUnoPlugin
{
public:
	SDRunoPlugin_Cosmo(IUnoPluginController& controller);
	virtual ~SDRunoPlugin_Cosmo();

	// IUnoPlugin
	virtual const char* GetPluginName() const override;
	virtual double GetPluginVersion() const override;
	virtual const char* GetPluginVendor() const override;
	virtual const char* GetPluginDescription() const override;
	virtual const int GetSDRplayPluginApi() const override;
	virtual const char* GetPluginId() const override;
	virtual void HandleEvent(const UnoEvent& ev) override;
	virtual bool HasForm() const override;
	virtual bool CanProcessNotifications() const override;
	virtual bool HasMenuHandler() const override;
	virtual bool HasInfoDisplay() const override;
	virtual void ShowUi() override;
	virtual void HideUi() override;

	// IUnoStreamObserver
	virtual void StreamProcessingStarted() override;
	virtual void StreamProcessingStopped() override;
	virtual void StreamProcessingFlushed() override;

	// IUnoAudioObserver  
	virtual void AudioProcessingStarted() override;
	virtual void AudioProcessingStopped() override;

private:
	IUnoPluginController& m_controller;
	std::shared_ptr<SDRunoPlugin_CosmoUi> m_ui;
	bool m_isStreamProcessing;
	bool m_isAudioProcessing;
};

extern "C" {
	UNOPLUGINAPI IUnoPlugin* UNOPLUGINCALL CreatePlugin(IUnoPluginController& controller);
}