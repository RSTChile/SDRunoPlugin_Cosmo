#include <windows.h>
#include "SDRunoPlugin_Template.h"
#include "SDRunoPlugin_TemplateUi.h"
#include "iunoplugincontroller.h"

// Instancia global del plugin y UI
SDRunoPlugin_Template* g_plugin = nullptr;
SDRunoPlugin_TemplateUi* g_ui = nullptr;

// Punto de entrada estándar de DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Aquí deberías instanciar el plugin y la UI solo si tienes acceso a IUnoPluginController
        // Ejemplo (real, depende de tu framework):
        // extern IUnoPluginController* g_controller; // Debe estar definido en tu app principal
        // g_plugin = new SDRunoPlugin_Template(*g_controller);
        // g_ui = new SDRunoPlugin_TemplateUi(*g_plugin, *g_controller);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        // Limpieza
        if (g_ui) {
            delete g_ui;
            g_ui = nullptr;
        }
        if (g_plugin) {
            delete g_plugin;
            g_plugin = nullptr;
        }
        break;
    }
    return TRUE;
}
