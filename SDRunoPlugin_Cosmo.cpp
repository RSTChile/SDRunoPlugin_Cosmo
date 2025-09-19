#include "SDRunoPlugin_Cosmo.h"
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <iostream>

// Plugin Info
static const char* plugin_id = "RSTChile.SDRunoPlugin.Cosmo";
static const char* plugin_name = "Cosmo Plugin";
static const char* plugin_vendor = "RSTChile";
static const char* plugin_description = "SDRuno Cosmo Plugin for advanced signal processing";
static const double plugin_version = 1.0;

SDRunoPlugin_Cosmo::SDRunoPlugin_Cosmo(IUnoPluginController& controller) :
	IUnoPlugin(controller),
	m_controller(controller),
	m_isStreamProcessing(false),
	m_isAudioProcessing(false)
{
	m_ui = std::make_shared<SDRunoPlugin_CosmoUi>(m_controller, *this);
}

SDRunoPlugin_Cosmo::~SDRunoPlugin_Cosmo()
{
}

const char* SDRunoPlugin_Cosmo::GetPluginName() const
{
	return plugin_name;
}

double SDRunoPlugin_Cosmo::GetPluginVersion() const
{
	return plugin_version;
}

const char* SDRunoPlugin_Cosmo::GetPluginVendor() const
{
	return plugin_vendor;
}

const char* SDRunoPlugin_Cosmo::GetPluginDescription() const
{
	return plugin_description;
}

const int SDRunoPlugin_Cosmo::GetSDRplayPluginApi() const
{
	return SDRUNO_PLUGIN_API;
}

const char* SDRunoPlugin_Cosmo::GetPluginId() const
{
	return plugin_id;
}

void SDRunoPlugin_Cosmo::HandleEvent(const UnoEvent& ev)
{
	if (m_ui != nullptr)
	{
		m_ui->HandleEvent(ev);
	}
}

bool SDRunoPlugin_Cosmo::HasForm() const
{
	return true;
}

bool SDRunoPlugin_Cosmo::CanProcessNotifications() const
{
	return true;
}

bool SDRunoPlugin_Cosmo::HasMenuHandler() const
{
	return false;
}

bool SDRunoPlugin_Cosmo::HasInfoDisplay() const
{
	return false;
}

void SDRunoPlugin_Cosmo::ShowUi()
{
	if (m_ui != nullptr)
	{
		m_ui->ShowUi();
	}
}

void SDRunoPlugin_Cosmo::HideUi()
{
	if (m_ui != nullptr)
	{
		m_ui->HideUi();
	}
}

void SDRunoPlugin_Cosmo::StreamProcessingStarted()
{
	m_isStreamProcessing = true;
}

void SDRunoPlugin_Cosmo::StreamProcessingStopped()
{
	m_isStreamProcessing = false;
}

void SDRunoPlugin_Cosmo::StreamProcessingFlushed()
{
	// Handle stream flush if needed
}

void SDRunoPlugin_Cosmo::AudioProcessingStarted()
{
	m_isAudioProcessing = true;
}

void SDRunoPlugin_Cosmo::AudioProcessingStopped()
{
	m_isAudioProcessing = false;
}

extern "C" {
	UNOPLUGINAPI IUnoPlugin* UNOPLUGINCALL CreatePlugin(IUnoPluginController& controller)
	{
		return new SDRunoPlugin_Cosmo(controller);
	}
}