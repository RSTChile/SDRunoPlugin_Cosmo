#include <iunoplugin.h>
#include "SDRunoPlugin_Template.h"

// Endurece los exports: extern "C", convención correcta por macro del SDK
// y captura de excepciones para que nada cruce el límite de la DLL hacia el host.

extern "C"
{
    UNOPLUGINAPI IUnoPlugin* UNOPLUGINCALL CreatePlugin(IUnoPluginController& controller)
    {
        try {
            return new SDRunoPlugin_Template(controller);
        } catch (...) {
            // Nunca dejar escapar excepciones al host (Delphi).
            // Si algo falla al crear, devuelve nullptr.
            return nullptr;
        }
    }

    UNOPLUGINAPI void UNOPLUGINCALL DestroyPlugin(IUnoPlugin* plugin)
    {
        try {
            delete plugin;
        } catch (...) {
            // El host no puede manejar excepciones C++; trágatelas aquí.
        }
    }

    UNOPLUGINAPI unsigned int UNOPLUGINCALL GetPluginApiLevel()
    {
        // Simple y seguro
        return UNOPLUGINAPIVERSION;
    }
}
