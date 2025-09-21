#include <iunoplugin.h>
#include "SDRunoPlugin_Template.h"

extern "C" {

    UNOPLUGINAPI IUnoPlugin* UNOPLUGINCALL CreatePlugin(IUnoPluginController& controller)
    {
        try {
            return new SDRunoPlugin_Template(controller);
        } catch (...) {
            // Nunca dejar cruzar excepciones al host (Delphi)
            return nullptr;
        }
    }

    UNOPLUGINAPI void UNOPLUGINCALL DestroyPlugin(IUnoPlugin* plugin)
    {
        try {
            delete plugin;
        } catch (...) {
            // El host no puede manejar excepciones C++; tragarlas
        }
    }

    UNOPLUGINAPI unsigned int UNOPLUGINCALL GetPluginApiLevel()
    {
        return UNOPLUGINAPIVERSION;
    }
}
