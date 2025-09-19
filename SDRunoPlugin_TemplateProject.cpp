#include <iunoplugin.h>
#include "SDRunoPlugin_Template.h"

extern "C"
{

	UNOPLUGINAPI IUnoPlugin* UNOPLUGINCALL CreatePlugin(IUnoPluginController& controller)
	{
		return new SDRunoPlugin_Template(controller);
	}

	UNOPLUGINAPI void UNOPLUGINCALL DestroyPlugin(IUnoPlugin* plugin)
	{
		delete plugin;
	}

	UNOPLUGINAPI unsigned int UNOPLUGINCALL GetPluginApiLevel()
	{
		return UNOPLUGINAPIVERSION;
	}
}

