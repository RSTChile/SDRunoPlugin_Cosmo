#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_Template& parent, IUnoPluginController& controller, SDRunoPlugin_TemplateUi& ui)
    : nana::form(nana::API::make_center(formWidth, formHeight)),
      m_parent(parent), m_controller(controller), m_ui(ui)
{
    Setup();
}

SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm() {}

void SDRunoPlugin_TemplateForm::Setup() {
    caption("SDRuno Cosmo Plugin");

    title_bar_label.caption("Cosmo Plugin");
    title_bar_label.move(nana::point(10, 8));
    title_bar_label.size(nana::size(160, 18));

    versionLbl.caption("v1.0");
    versionLbl.text_align(nana::align::right, nana::align_v::center);

    rcLabel.caption("RC:");
    inrLabel.caption("INR:");
    lfLabel.caption("LF:");
    rdeLabel.caption("RDE:");
    msgLabel.caption("");

    estadoCaption.caption("Estado:");
    estadoValor.caption("Sin Señal");

    // Combo modo
    modeCombo.push_back("Modo Restrictivo");
    modeCombo.push_back("Modo Funcional-Libre");
    modeCombo.option(0);
    modeCombo.size(nana::size(220, 26));

    modeCombo.events().selected([this](const nana::arg_combox&) {
        bool restrictivo = (modeCombo.option() == 0);
        m_parent.SetModeRestrictivo(restrictivo);
    });

    // Botón Settings: usar el UI manager (crea el diálogo con 3 parámetros)
    settingsBtn.caption("Settings");
    settingsBtn.events().click([this]() {
        m_ui.ShowSettingsDialog();
    });

    // Botón Capturar
    captureBtn.caption("Capturar");
    captureBtn.events().click([this]() {
        m_capturing = !m_capturing;
        m_ui.ToggleCapture(m_capturing);
        captureBtn.caption(m_capturing ? "Detener" : "Capturar");
        // Refrescar estado
        SetSavePath(m_currentPath);
    });

    // Cierre de ventana principal
    events().unload([this](const nana::arg_unload& /*arg*/) {
        m_ui.FormClosed();
    });

    CreateLedBars();
}

void SDRunoPlugin_TemplateForm::CreateLedBars() {
    auto make_bar = [&](int y, std::vector<std::unique_ptr<nana::panel<true>>>& barVec) {
        barVec.clear();
        for (int i = 0; i < LEDS; ++i) {
            int x = barStartX + i * (ledSize + ledGap);
            auto p = std::make_unique<nana::panel<true>>(*this, nana::rectangle(x, y, ledSize, ledSize));
            p->bgcolor(nana::colors::gray_border);
            p->transparent(false);
            p->show();
            barVec.emplace_back(std::move(p));
        }
    };

    make_bar(70, rcBar);
    make_bar(100, inrBar);
    make_bar(130, lfBar);
    make_bar(160, rdeBar);
}

void SDRunoPlugin_TemplateForm::Run() {
    this->show();
    nana::exec();
}

void SDRunoPlugin_TemplateForm::UpdateBar(std::vector<std::unique_ptr<nana::panel<true>>>& bar, float value, const nana::color& on, const nana::color& off) {
    // Clamp 0..1
    value = std::max(0.0f, std::min(1.0f, value));
    int lit = static_cast<int>(value * LEDS + 0.5f);
    if (lit > LEDS) lit = LEDS;
    for (int i = 0; i < LEDS; ++i) {
        bar[i]->bgcolor(i < lit ? on : off);
        // Forzar repintado de cada “LED”
        nana::API::refresh_window(bar[i]->handle());
    }
}

void SDRunoPlugin_TemplateForm::UpdateMetrics(float rc, float inr, float lf, float rde, const std::string& msg, bool modoRestrictivo) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);

    oss.str(""); oss.clear(); oss << "RC: " << rc;
    rcLabel.caption(oss.str());
    UpdateBar(rcBar, rc, nana::colors::green, nana::color_rgb(0x303030));

    oss.str(""); oss.clear(); oss << "INR: " << inr;
    inrLabel.caption(oss.str());
    UpdateBar(inrBar, inr, nana::colors::orange, nana::color_rgb(0x303030));

    oss.str(""); oss.clear(); oss << "LF: " << lf;
    lfLabel.caption(oss.str());
    UpdateBar(lfBar, lf, nana::colors::dark_blue, nana::color_rgb(0x303030));

    oss.str(""); oss.clear(); oss << "RDE: " << rde;
    rdeLabel.caption(oss.str());
    UpdateBar(rdeBar, rde, nana::colors::red, nana::color_rgb(0x303030));

    msgLabel.caption(msg);

    // Estilos del mensaje según modo
    if (modoRestrictivo) {
        msgLabel.bgcolor(nana::colors::white);
        msgLabel.fgcolor(nana::colors::black);
    } else {
        msgLabel.bgcolor(nana::colors::light_yellow);
        msgLabel.fgcolor(nana::colors::dark_blue);
    }

    // Forzar repintado de la ventana tras actualizar métricas
    nana::API::refresh_window(handle());
}

void SDRunoPlugin_TemplateForm::SettingsButton_Click() {
    if (!m_settingsDialog) {
        m_settingsDialog = std::make_shared<SDRunoPlugin_TemplateSettingsDialog>(m_ui, *this, m_controller);
    }
    m_settingsDialog->show();
}

void SDRunoPlugin_TemplateForm::SetSavePath(const std::string& path) {
    m_currentPath = path;
    if (!m_streaming) {
        estadoValor.caption("Sin Señal");
    } else {
        if (!m_capturing) {
            estadoValor.caption("Streaming activo (captura detenida)");
        } else {
            if (m_currentPath.empty())
                estadoValor.caption("Captura de Señal");
            else
                estadoValor.caption("Captura de Señal: " + m_currentPath);
        }
    }
    // Refrescar estado mostrado
    nana::API::refresh_window(handle());
}

void SDRunoPlugin_TemplateForm::SetStreaming(bool streaming) {
    m_streaming = streaming;
    SetSavePath(m_currentPath);
}
