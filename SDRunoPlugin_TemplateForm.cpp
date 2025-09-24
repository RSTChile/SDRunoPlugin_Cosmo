#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "SDRunoPlugin_TemplateForm.h"
#include "SDRunoPlugin_TemplateSettingsDialog.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "resource.h"
#include <io.h>
#include <shlobj.h>

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

// Inicializa todos los controles del formulario.
void SDRunoPlugin_TemplateForm::Setup()
{
    using namespace nana;

    // Cargar imágenes desde la carpeta "resources". Si prefieres incrustarlas
    // en el RC, usa FindResource/LoadImage como en el plugin DRM [oai_citation:2‡raw.githubusercontent.com](https://raw.githubusercontent.com/JvanKatwijk/SDRunoPlugin_drm/master/SDRunoPlugin_drmForm.cpp).
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

    // Colocar el header y los botones en sus posiciones
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
    title_bar_label.align(nana::align::left, nana::align_v::center);

    versionLbl.caption(VERSION);
    versionLbl.bgcolor(colors::transparent);
    versionLbl.fgcolor(colors::white);

    // Preparar LED (mantiene las rutas originales)
    ledOnImg.open("resources\\led_on.bmp");
    ledOffImg.open("resources\\led_off.bmp");
    ledPicture.load(ledOffImg, rectangle(0, 0, 20, 20));
    ledPicture.transparent(true);

    // Permitir arrastrar la ventana haciendo clic en cualquier zona del formulario
    form_drag_label.bgcolor(colors::transparent);
    form_dragger.target(*this);
    form_dragger.trigger(form_drag_label);

    // Eventos de botón Close
    close_button.events().mouse_down([&](const arg_mouse&) {
        if (!img_close_down.empty()) close_button.load(img_close_down);
    });
    close_button.events().mouse_up([&](const arg_mouse&) {
        if (!img_close_normal.empty()) close_button.load(img_close_normal);
    });
    close_button.events().click([&](const arg_click&) {
        // Notificar al controlador para descargar el plugin
        m_controller.RequestUnload(&m_parent);
    });

    // Eventos de botón Minimize
    min_button.events().mouse_down([&](const arg_mouse&) {
        if (!img_min_down.empty()) min_button.load(img_min_down);
    });
    min_button.events().mouse_up([&](const arg_mouse&) {
        if (!img_min_normal.empty()) min_button.load(img_min_normal);
    });
    min_button.events().click([&](const arg_click&) {
        // Minimizar la ventana
        this->minimize(true);
    });

    // Eventos de botón Settings
    sett_button.events().mouse_down([&](const arg_mouse&) {
        if (!img_sett_down.empty()) sett_button.load(img_sett_down);
    });
    sett_button.events().mouse_up([&](const arg_mouse&) {
        if (!img_sett_normal.empty()) sett_button.load(img_sett_normal);
    });
    sett_button.events().click([&](const arg_click&) {
        SettingsButton_Click();
    });
}

// Cambia el estado del LED (encendido/apagado).
void SDRunoPlugin_TemplateForm::SetLedState(bool on)
{
    if (on) {
        ledPicture.load(ledOnImg, nana::rectangle(0, 0, 20, 20));
    } else {
        ledPicture.load(ledOffImg, nana::rectangle(0, 0, 20, 20));
    }
}

// Evento del botón de configuración: abre el diálogo de Settings.
void SDRunoPlugin_TemplateForm::SettingsButton_Click()
{
    // Crear el diálogo de configuración y desactivar la ventana principal mientras está abierto
    auto settingsDialog = new SDRunoPlugin_TemplateSettingsDialog(m_parent, *this, m_controller);
    this->enabled(false);
    settingsDialog->show();
    // Puedes conectar el cierre del diálogo para volver a habilitar la ventana:
    // settingsDialog->events().destroy([&]{ SettingsDialog_Closed(); });
}

void SDRunoPlugin_TemplateForm::SettingsDialog_Closed()
{
    this->enabled(true);
    this->focus();
}