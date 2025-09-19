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

// Form constructor con handles a parent y controller - lanza Setup
SDRunoPlugin_TemplateForm::SDRunoPlugin_TemplateForm(SDRunoPlugin_TemplateUi& parent, IUnoPluginController& controller) :
    nana::form(nana::API::make_center(formWidth, formHeight), nana::appearance(false, true, false, false, true, false, false)),
    m_parent(parent),
    m_controller(controller)
{
    Setup();
}

// Form destructor
SDRunoPlugin_TemplateForm::~SDRunoPlugin_TemplateForm()
{
}

// Start Form y comienza Nana UI
void SDRunoPlugin_TemplateForm::Run()
{
    show();
    nana::exec();
}

// Crear el formulario inicial del plugin (versión sin bitmaps, todo funcional y visible)
void SDRunoPlugin_TemplateForm::Setup()
{
    // Tamaño y título
    size(nana::size(formWidth, formHeight));
    caption("SDRuno Plugin Cosmo");

    // Fondo negro estilo SDRuno
    bgcolor(nana::colors::black);
    bg_border.size(nana::size(formWidth, formHeight));
    bg_border.bgcolor(nana::colors::black);

    // Área interior
    bg_inner.move(nana::point(sideBorderWidth, topBarHeight));
    bg_inner.size(nana::size(formWidth - (2 * sideBorderWidth), formHeight - topBarHeight - bottomBarHeight));
    bg_inner.bgcolor(nana::colors::black);

    // Evento de cierre del formulario principal -> descargar plugin
    events().destroy([&] { m_parent.FormClosed(); });

    // Barra y título
    header_bar.size(nana::size(122, 20));
    header_bar.move(nana::point((formWidth / 2) - 61, 5));
    header_bar.transparent(true);

    title_bar_label.size(nana::size(120, 16));
    title_bar_label.move(nana::point((formWidth / 2) - 60, 7));
    title_bar_label.format(true);
    title_bar_label.caption("< bold size = 9 color = 0xFFFFFF font = \"Verdana\">COSMO</>");
    title_bar_label.text_align(nana::align::center, nana::align_v::center);
    title_bar_label.transparent(true);

    // Versión
    versionLbl.caption(VERSION);
    versionLbl.fgcolor(nana::colors::white);
    versionLbl.transparent(true);

    // Hacer la ventana arrastrable desde cualquier parte
    form_drag_label.transparent(true);
    form_drag_label.size(nana::size(formWidth, formHeight));
    form_dragger.target(*this);
    form_dragger.trigger(form_drag_label);

    // Botón Cerrar (área clickeable)
    close_button.move(nana::point(formWidth - 26, 6));
    close_button.size(nana::size(20, 15));
    close_button.transparent(true);
    close_button.events().click([this] {
        this->close();
    });

    // Botón Minimizar (área clickeable)
    min_button.move(nana::point(formWidth - 50, 6));
    min_button.size(nana::size(20, 15));
    min_button.transparent(true);
    min_button.events().click([this] {
        nana::API::zoom_window(*this, nana::window_zoom::minimized);
    });

    // Botón Settings (área clickeable)
    sett_button.move(nana::point(10, 6));
    sett_button.size(nana::size(40, 15));
    sett_button.transparent(true);
    sett_button.tooltip("Settings");
    sett_button.events().click([this] {
        SettingsButton_Click();
    });

    // Sugerencia visual mínima (sin bitmaps): dibujar líneas de contorno
    // Solo decorativo para que no se vea completamente plano
    nana::drawing draw_bg(bg_border);
    draw_bg.draw([this](nana::paint::graphics& graph) {
        // Borde exterior gris
        graph.rectangle(true, nana::color_rgb(0x101010));
        graph.rectangle(false, nana::color_rgb(0x404040));
        // Línea superior simulando barra
        graph.rectangle(nana::rectangle(0, 0, formWidth, topBarHeight), false, nana::color_rgb(0x606060));
    });
    draw_bg.update();
}

void SDRunoPlugin_TemplateForm::SettingsButton_Click()
{
    // Abrir el diálogo de Settings de forma no modal
    // Se crea en el heap para mantenerlo vivo hasta que el usuario lo cierre
    auto* dlg = new SDRunoPlugin_TemplateSettingsDialog(m_parent, m_controller);
    dlg->events().destroy([this, dlg] {
        SettingsDialog_Closed();
        delete dlg;
    });
    dlg->show();
}

void SDRunoPlugin_TemplateForm::SettingsDialog_Closed()
{
    // Aquí no hay que hacer nada especial por ahora
}
