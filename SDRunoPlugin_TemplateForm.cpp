#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"

#define VERSION "V1.1"

SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent,
                                                     IUnoPluginController& controller)
    : nana::form(nana::API::make_center(formWidth, formHeight),
                 nana::appearance(false, true, false, false, true, false, false)),
      m_parent(parent),
      m_controller(controller)
{
    Setup();
}

SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm()
{
}

void SDRunoPlugin_TemplateForm::Run()
{
    show();
    nana::exec();
}

void SDRunoPlugin_TemplateForm::Setup()
{
    using namespace nana;

    // Cargar imágenes desde la carpeta "resources"
    img_bg_border.open("resources\\winback.bmp");
    img_bg_inner.open("resources\\sp_blue_back.bmp");
    img_header.open("resources\\HEADER.dib");
    img_min_normal.open("resources\\MIN_BUT.bmp");
    img_min_down.open("resources\\MIN_BUT_HIT.bmp");
    img_close_normal.open("resources\\X_BUT.bmp");
    img_close_down.open("resources\\X_BUT_HIT.bmp");
    img_sett_normal.open("resources\\SETT.bmp");
    img_sett_down.open("resources\\SETT_OVER.bmp");

    // Asignar imágenes a los controles
    if (!img_bg_border.empty())  bg_border.load(img_bg_border);
    if (!img_bg_inner.empty())   bg_inner.load(img_bg_inner);
    if (!img_header.empty())     header_bar.load(img_header);
    if (!img_min_normal.empty()) min_button.load(img_min_normal);
    if (!img_close_normal.empty()) close_button.load(img_close_normal);
    if (!img_sett_normal.empty()) sett_button.load(img_sett_normal);

    // Posicionar elementos superiores
    header_bar.move(rectangle(sideBorderWidth, 0,
                              formWidth - 2 * sideBorderWidth,
                              topBarHeight));
    close_button.move(rectangle(formWidth - sideBorderWidth - 20, 0, 20, topBarHeight));
    min_button.move(rectangle(formWidth - sideBorderWidth - 40, 0, 20, topBarHeight));
    sett_button.move(rectangle(formWidth - sideBorderWidth - 80, 0, 40, topBarHeight));

    // Configurar título y versión
    title_bar_label.caption("Cosmo");
    title_bar_label.move(rectangle(sideBorderWidth + 5, 0, 150, topBarHeight));
    title_bar_label.fgcolor(colors::white);
    versionLbl.caption(VERSION);
    versionLbl.bgcolor(colors::transparent);
    versionLbl.fgcolor(colors::white);

    // Preparar LED
    ledOnImg.open("resources\\led_on.bmp");
    ledOffImg.open("resources\\led_off.bmp");
    ledPicture.load(ledOffImg, rectangle(0, 0, 20, 20));
    // Nota: no usamos ledPicture.transparent(true) porque no está soportado en esta versión de Nana

    // Permitir arrastrar la ventana
    form_dragger.target(*this);
    form_dragger.trigger(form_drag_label);

    // Eventos para el botón Close
    close_button.events().mouse_down([&] {
        if (!img_close_down.empty()) close_button.load(img_close_down);
    });
    close_button.events().mouse_up([&] {
        if (!img_close_normal.empty()) close_button.load(img_close_normal);
    });
    close_button.events().click([&] {
        // Notificar al UI que la ventana ha sido cerrada
        m_parent.FormClosed();
    });

    // Eventos para el botón Minimize
    min_button.events().mouse_down([&] {
        if (!img_min_down.empty()) min_button.load(img_min_down);
    });
    min_button.events().mouse_up([&] {
        if (!img_min_normal.empty()) min_button.load(img_min_normal);
    });
    min_button.events().click([&] {
        // Minimizar la ventana utilizando la API de Nana
        nana::API::zoom_window(this->handle(), false);
    });

    // Eventos para el botón Settings
    sett_button.events().mouse_down([&] {
        if (!img_sett_down.empty()) sett_button.load(img_sett_down);
    });
    sett_button.events().mouse_up([&] {
        if (!img_sett_normal.empty()) sett_button.load(img_sett_normal);
    });
    sett_button.events().click([&] {
        SettingsButton_Click();
    });
}

void SDRunoPlugin_TemplateForm::SetLedState(bool on)
{
    if (on) {
        ledPicture.load(ledOnImg, nana::rectangle(0, 0, 20, 20));
    } else {
        ledPicture.load(ledOffImg, nana::rectangle(0, 0, 20, 20));
    }
}

void SDRunoPlugin_TemplateForm::SettingsButton_Click()
{
    // Crear y mostrar el diálogo de configuración
    auto settingsDialog = new SDRunoPlugin_TemplateSettingsDialog(m_parent, *this, m_controller);
    this->enabled(false);
    settingsDialog->show();
    // Cuando se cierre el diálogo, se debe volver a habilitar la ventana
    // Puedes conectar events().destroy en settingsDialog para llamar a SettingsDialog_Closed
}

void SDRunoPlugin_TemplateForm::SettingsDialog_Closed()
{
    // Rehabilitar la ventana al cerrar el diálogo
    this->enabled(true);
    this->focus();
}