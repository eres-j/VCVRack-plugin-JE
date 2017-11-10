#include "JE.hpp"

rack::Plugin* plugin;

void init(rack::Plugin *p)
{
	plugin = p;
	plugin->slug = "JE";
	plugin->version = TOSTRING(VERSION);

	p->addModel(rack::createModel<RingModulatorWidget>(
		TOSTRING(MANUFACTURE_SLUG), TOSTRING(MANUFACTURE_NAME),
		"RingModulator, "Ring Modulator"));
}
