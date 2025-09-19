#include "SDRunoPlugin_CosmoUi.h"
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <iostream>

SDRunoPlugin_CosmoUi::SDRunoPlugin_CosmoUi(IUnoPluginController& controller, IUnoAudioProcessor& processor) :
	m_controller(controller),
	m_processor(processor),
	m_started(false)
{
	// Initialize form on main thread
	m_form = std::make_shared<nana::form>(nana::API::make_center(450, 300));
	m_form->caption("Cosmo Plugin v1.0");
	Setup();
}

SDRunoPlugin_CosmoUi::~SDRunoPlugin_CosmoUi()
{
	Stop();
	FormClosed();
}

void SDRunoPlugin_CosmoUi::Setup()
{
	// Create UI controls
	m_frequencyLabel = std::make_shared<nana::label>(*m_form, nana::rectangle(10, 10, 100, 25));
	m_frequencyLabel->caption("Frequency:");

	m_frequencyTextbox = std::make_shared<nana::textbox>(*m_form, nana::rectangle(120, 10, 150, 25));
	m_frequencyTextbox->caption("100000000"); // Default 100MHz

	m_startButton = std::make_shared<nana::button>(*m_form, nana::rectangle(10, 50, 80, 30));
	m_startButton->caption("Start");

	m_stopButton = std::make_shared<nana::button>(*m_form, nana::rectangle(100, 50, 80, 30));
	m_stopButton->caption("Stop");
	m_stopButton->enabled(false);

	m_statusLabel = std::make_shared<nana::label>(*m_form, nana::rectangle(10, 100, 400, 25));
	m_statusLabel->caption("Status: Ready");

	// Set up event handlers
	m_startButton->events().click([this] { OnStartClicked(); });
	m_stopButton->events().click([this] { OnStopClicked(); });
	m_frequencyTextbox->events().text_changed([this] { OnFrequencyChanged(); });

	// Set up form close event
	m_form->events().unload([this](const nana::arg_unload& arg) {
		FormClosed();
	});
}

void SDRunoPlugin_CosmoUi::HandleEvent(const UnoEvent& ev)
{
	switch (ev.GetType())
	{
	case UnoEvent::FrequencyChanged:
		SetFrequency(ev.GetFrequency());
		break;
	case UnoEvent::RateChanged:
		// Handle sample rate change
		break;
	default:
		break;
	}
}

void SDRunoPlugin_CosmoUi::FormClosed()
{
	Stop();
	if (m_form)
	{
		m_form->close();
	}
}

void SDRunoPlugin_CosmoUi::ShowUi()
{
	if (m_form)
	{
		m_form->show();
		m_form->focus();
	}
}

void SDRunoPlugin_CosmoUi::HideUi()
{
	if (m_form)
	{
		m_form->hide();
	}
}

bool SDRunoPlugin_CosmoUi::IsUiVisible()
{
	if (m_form)
	{
		return m_form->visible();
	}
	return false;
}

void SDRunoPlugin_CosmoUi::SetFrequency(long long frequency)
{
	if (m_frequencyTextbox)
	{
		m_frequencyTextbox->caption(std::to_string(frequency));
	}
}

void SDRunoPlugin_CosmoUi::Start()
{
	std::lock_guard<std::mutex> lock(m_lock);
	
	if (!m_started)
	{
		m_started = true;
		
		if (m_startButton) m_startButton->enabled(false);
		if (m_stopButton) m_stopButton->enabled(true);
		
		UpdateStatus("Status: Started");
		
		// Start processing thread
		m_thread = std::thread([this]() {
			// Plugin processing logic would go here
			while (m_started)
			{
				// Process audio/IQ data
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		});
	}
}

void SDRunoPlugin_CosmoUi::Stop()
{
	std::lock_guard<std::mutex> lock(m_lock);
	
	if (m_started)
	{
		m_started = false;
		
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		
		if (m_startButton) m_startButton->enabled(true);
		if (m_stopButton) m_stopButton->enabled(false);
		
		UpdateStatus("Status: Stopped");
	}
}

void SDRunoPlugin_CosmoUi::UpdateStatus(const std::string& status)
{
	if (m_statusLabel)
	{
		m_statusLabel->caption(status);
	}
}

void SDRunoPlugin_CosmoUi::OnStartClicked()
{
	Start();
}

void SDRunoPlugin_CosmoUi::OnStopClicked()
{
	Stop();
}

void SDRunoPlugin_CosmoUi::OnFrequencyChanged()
{
	if (m_frequencyTextbox)
	{
		try
		{
			long long freq = std::stoll(m_frequencyTextbox->caption());
			// Notify controller of frequency change if needed
			// m_controller.SetFrequency(freq);
		}
		catch (const std::exception&)
		{
			// Invalid frequency format
		}
	}
}