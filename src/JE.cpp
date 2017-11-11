#include "JE.hpp"

rack::Plugin* plugin;

void init(rack::Plugin *p)
{
	plugin = p;
	plugin->slug = "JE";
	plugin->name = TOSTRING(MANUFACTURE_NAME);
	//plugin->version = TOSTRING(JE_VERSION);

	rack::createModel<RingModulatorWidget>(p,
		/*TOSTRING(MANUFACTURE_SLUG), TOSTRING(MANUFACTURE_NAME),*/
		"RingModulator", "Ring Modulator");
}
