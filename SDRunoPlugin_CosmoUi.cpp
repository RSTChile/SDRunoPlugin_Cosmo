#include "SDRunoPlugin_CosmoUi.h"
#include <chrono>

SDRunoPlugin_CosmoUi::SDRunoPlugin_CosmoUi(IUnoPluginController& controller, IUnoPlugin& plugin)
    : m_controller(controller)
    , m_plugin(plugin)
{
    // Crear formulario en hilo de GUI
    m_form = std::make_shared<nana::form>(nana::API::make_center(450, 200));
    m_form->caption("Cosmo Plugin v1.0 - RSTChile");

    Setup();

    // Timer para refrescar UI con datos del worker sin tocar widgets desde el thread
    m_uiTimer.elapse([this] {
        // Este callback se ejecuta en el hilo de GUI
        if (m_samplesLabel) {
            UpdateSampleCountLabel(m_sampleCount.load(std::memory_order_relaxed));
        }
    });
    m_uiTimer.interval(200); // refresco 5 Hz
    m_uiTimer.start();
}

SDRunoPlugin_CosmoUi::~SDRunoPlugin_CosmoUi()
{
    Stop(); // garantiza join del hilo antes de destruir widgets
}

void SDRunoPlugin_CosmoUi::Setup()
{
    // Controles
    m_frequencyLabel = std::make_shared<nana::label>(*m_form, nana::rectangle(10, 10, 100, 25));
    m_frequencyLabel->caption("Frequency (Hz):");

    m_frequencyTextbox = std::make_shared<nana::textbox>(*m_form, nana::rectangle(120, 10, 150, 25));
    m_frequencyTextbox->caption("100000000"); // 100 MHz

    m_gainLabel = std::make_shared<nana::label>(*m_form, nana::rectangle(280, 10, 60, 25));
    m_gainLabel->caption("Gain:");

    m_gainTextbox = std::make_shared<nana::textbox>(*m_form, nana::rectangle(350, 10, 80, 25));
    m_gainTextbox->caption("1.0");

    m_startButton = std::make_shared<nana::button>(*m_form, nana::rectangle(10, 50, 80, 30));
    m_startButton->caption("Start");

    m_stopButton = std::make_shared<nana::button>(*m_form, nana::rectangle(100, 50, 80, 30));
    m_stopButton->caption("Stop");
    m_stopButton->enabled(false);

    m_resetButton = std::make_shared<nana::button>(*m_form, nana::rectangle(190, 50, 80, 30));
    m_resetButton->caption("Reset");

    m_statusLabel = std::make_shared<nana::label>(*m_form, nana::rectangle(10, 100, 400, 25));
    m_statusLabel->caption("Status: Ready");

    m_samplesLabel = std::make_shared<nana::label>(*m_form, nana::rectangle(10, 130, 400, 25));
    m_samplesLabel->caption("Samples processed: 0");

    // Eventos (siempre en hilo GUI)
    m_startButton->events().click([this] { OnStartClicked(); });
    m_stopButton->events().click([this] { OnStopClicked(); });
    m_resetButton->events().click([this] { OnResetClicked(); });
    m_frequencyTextbox->events().text_changed([this] { OnFrequencyChanged(); });
    m_gainTextbox->events().text_changed([this] { OnGainChanged(); });

    // Cierre del formulario -> parar hilo con join antes de destruir
    m_form->events().unload([this](const nana::arg_unload& arg) {
        Stop();
    });
}

void SDRunoPlugin_CosmoUi::Show()
{
    if (m_form) m_form->show();
}

void SDRunoPlugin_CosmoUi::Hide()
{
    if (m_form) m_form->hide();
}

void SDRunoPlugin_CosmoUi::Start()
{
    if (m_started.exchange(true)) {
        return; // ya estaba iniciado
    }

    if (m_startButton) m_startButton->enabled(false);
    if (m_stopButton)  m_stopButton->enabled(true);

    UpdateStatus("Status: Started - Processing signals...");
    m_sampleCount.store(0, std::memory_order_relaxed);

    // Hilo de trabajo: NO tocar UI aquí
    m_thread = std::thread([this]() noexcept {
        try {
            auto last = std::chrono::steady_clock::now();
            while (m_started.load(std::memory_order_relaxed)) {
                // Simular procesamiento
                m_sampleCount.fetch_add(1024, std::memory_order_relaxed);

                // Simular carga de trabajo
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                // Si se necesitara lógica adicional basada en tiempo, usar 'last'
                (void)last;
            }
        } catch (...) {
            // Evitar std::terminate; el error puede anotarse a un log si se dispone
        }
    });
}

void SDRunoPlugin_CosmoUi::Stop()
{
    bool wasRunning = m_started.exchange(false);
    if (wasRunning) {
        if (m_thread.joinable()) {
            m_thread.join(); // asegurar que ya no corre antes de tocar UI
        }
        if (m_startButton) m_startButton->enabled(true);
        if (m_stopButton)  m_stopButton->enabled(false);
        UpdateStatus("Status: Stopped");
    } else {
        // Aunque no estuviera corriendo, si hay un thread joinable, join por seguridad
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void SDRunoPlugin_CosmoUi::UpdateStatus(const std::string& text)
{
    // Se llama sólo desde GUI thread
    if (m_statusLabel) {
        m_statusLabel->caption(text);
    }
}

void SDRunoPlugin_CosmoUi::UpdateSampleCountLabel(long long samples)
{
    if (m_samplesLabel) {
        m_samplesLabel->caption("Samples processed: " + std::to_string(samples));
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
    if (m_frequencyTextbox) {
        try {
            auto freq = std::stoll(m_frequencyTextbox->caption());
            // Aplicar al controller (no UI)
            m_controller.SetFrequency(freq);
            UpdateStatus("Status: Frequency set to " + std::to_string(freq));
        } catch (...) {
            // valor inválido -> opcionalmente reflejar en estado
        }
    }
}

void SDRunoPlugin_CosmoUi::OnGainChanged()
{
    if (m_gainTextbox) {
        try {
            double gain = std::stod(m_gainTextbox->caption());
            UpdateStatus("Status: Gain updated to " + std::to_string(gain));
            // Aplicar a procesamiento si procede (en el worker, leer un atomic<double> con el gain)
        } catch (...) {
            UpdateStatus("Status: Invalid gain value");
        }
    }
}

void SDRunoPlugin_CosmoUi::OnResetClicked()
{
    if (m_frequencyTextbox) m_frequencyTextbox->caption("100000000");
    if (m_gainTextbox)      m_gainTextbox->caption("1.0");
    m_sampleCount.store(0, std::memory_order_relaxed);
    UpdateSampleCountLabel(0);
    UpdateStatus("Status: Reset to defaults");
}
