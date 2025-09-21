#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>

SDRunoPlugin_Template::SDRunoPlugin_Template(IUnoPluginController& controller)
    : IUnoPlugin(controller)
{
    // Registrar observador de IQ en el canal 0 (VRX por defecto)
    controller.RegisterStreamObserver(0, this);

    // Logging
    logFile.open("cosmo_metrics_log.csv", std::ios::out);
    logFile << "RC,INR,LF,RDE,MSG\n";
    // IMPORTANTE: No crear UI aquí. Se hará en diferido al primer callback IQ.
}

SDRunoPlugin_Template::~SDRunoPlugin_Template() {
    // Detener callbacks primero
    m_controller.UnregisterStreamObserver(0, this);

    // Destruir UI después de detener callbacks
    m_ui.reset();

    if (logFile.is_open()) logFile.close();
}

void SDRunoPlugin_Template::EnsureUiStarted() {
    if (!m_uiStarted.exchange(true)) {
        // Crear UI manager y arrancar su bucle/gestión interna
        m_ui = std::make_unique<SDRunoPlugin_TemplateUi>(*this, m_controller);
    }
}

void SDRunoPlugin_Template::RequestUnloadAsync() {
    m_unloadRequested.store(true, std::memory_order_release);
}

void SDRunoPlugin_Template::StreamObserverProcess(channel_t /*channel*/, const Complex* buffer, int length) {
    try {
        // Ejecutar unload de forma segura si fue pedido desde el hilo GUI
        if (m_unloadRequested.exchange(false, std::memory_order_acq_rel)) {
            try {
                m_controller.RequestUnload(this);
            } catch (...) {
                // Evitar propagar nada al host
            }
        }

        // Iniciar la UI en diferido al recibir las primeras muestras
        EnsureUiStarted();

        std::vector<float> iq;
        iq.reserve(static_cast<size_t>(length) * 2);
        for (int i = 0; i < length; ++i) {
            iq.push_back(static_cast<float>(buffer[i].real));
            iq.push_back(static_cast<float>(buffer[i].imag));
        }

        if (!haveRef) {
            UpdateReference(iq);
            haveRef = true;
        }

        float rc = CalculateRC(iq);
        float inr = CalculateINR(iq);
        float lf = CalculateLF(rc, inr);
        float rde = CalculateRDE(rc, inr);

        std::string palimpsestoMsg = DetectPalimpsesto(iq);

        std::string msg;
        if (!modoRestrictivo) {
            if (lf > 0.5f && rde > 0.3f) {
                msg = "¿Y si hay un patrón oculto? NO SÉ. DISIENTO.";
            } else if (lf < 0.05f) {
                msg = "NO SÉ: señal estéril.";
            } else if (!palimpsestoMsg.empty()) {
                msg = palimpsestoMsg;
            }
        } else {
            if (!palimpsestoMsg.empty()) msg = palimpsestoMsg;
        }

        UpdateUI(rc, inr, lf, rde, msg, modoRestrictivo);
        LogMetrics(rc, inr, lf, rde, msg);
    }
    catch (const std::exception& ex) {
        if (logFile.is_open()) {
            logFile << "0,0,0,0,\"EXCEPTION: " << ex.what() << "\"\n";
            logFile.flush();
        }
    }
    catch (...) {
        if (logFile.is_open()) {
            logFile << "0,0,0,0,\"EXCEPTION: unknown\"\n";
            logFile.flush();
        }
    }
}

float SDRunoPlugin_Template::CalculateRC(const std::vector<float>& iq) {
    size_t N = iq.size() / 2;
    float total = 0.0f, band = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float I = iq[2 * i];
        float Q = iq[2 * i + 1];
        float mag = std::sqrt(I * I + Q * Q);
        total += mag;
        if (i > N * 3 / 4) band += mag;
    }
    if (total == 0.0f) return 0.0f;
    return band / total;
}

float SDRunoPlugin_Template::CalculateINR(const std::vector<float>& iq) {
    size_t N = std::min(iq.size(), refSignal.size());
    if (N == 0) return 1.0f;
    float err = 0.0f, refMag = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float e = iq[i] - refSignal[i];
        err += e * e;
        refMag += refSignal[i] * refSignal[i];
    }
    if (refMag == 0.0f) return 1.0f;
    float rmse = std::sqrt(err / static_cast<float>(N));
    float norm = std::sqrt(refMag / static_cast<float>(N));
    return std::min(1.0f, rmse / (norm + 1e-6f));
}

float SDRunoPlugin_Template::CalculateLF(float rc, float inr) { return rc * (1.0f - inr); }
float SDRunoPlugin_Template::CalculateRDE(float rc, float inr) { return rc * inr; }

void SDRunoPlugin_Template::LogMetrics(float rc, float inr, float lf, float rde, const std::string& msg) {
    if (logFile.is_open()) {
        logFile << rc << "," << inr << "," << lf << "," << rde << ",\"" << msg << "\"\n";
        logFile.flush();
    }
}

void SDRunoPlugin_Template::UpdateReference(const std::vector<float>& iq) { refSignal = iq; }

std::string SDRunoPlugin_Template::DetectPalimpsesto(const std::vector<float>& iq) {
    size_t N = iq.size() / 2;
    int nPrimos = 0, nPicosEnPrimos = 0;
    for (size_t i = 2; i < N; ++i) {
        bool primo = true;
        for (size_t d = 2; d * d <= i; ++d) if (i % d == 0) { primo = false; break; }
        if (primo) {
            nPrimos++;
            float I = iq[2 * i];
            float Q = iq[2 * i + 1];
            float mag = std::sqrt(I * I + Q * Q);
            if (mag > 2.0f) nPicosEnPrimos++;
        }
    }
    if (nPrimos > 0 && (float)nPicosEnPrimos / nPrimos > 0.2f)
        return "Patrón palimpsesto detectado en índices primos.";
    return "";
}

void SDRunoPlugin_Template::SetModeRestrictivo(bool restrictivo) { modoRestrictivo = restrictivo; }
bool SDRunoPlugin_Template::GetModeRestrictivo() const { return modoRestrictivo; }

void SDRunoPlugin_Template::UpdateUI(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo) {
    if (m_ui) {
        m_ui->UpdateMetrics(rc, inr, lf, rde, msg, modoRestrictivo);
    }
}

// HandleEvent endurecido: jamás dejar escapar excepciones al host
void SDRunoPlugin_Template::HandleEvent(const UnoEvent& ev) {
    try {
        if (m_ui) {
            m_ui->HandleEvent(ev);
        }
    } catch (const std::exception& ex) {
        if (logFile.is_open()) {
            logFile << "0,0,0,0,\"HandleEvent EXCEPTION: " << ex.what() << "\"\n";
            logFile.flush();
        }
    } catch (...) {
        if (logFile.is_open()) {
            logFile << "0,0,0,0,\"HandleEvent EXCEPTION: unknown\"\n";
            logFile.flush();
        }
    }
}
