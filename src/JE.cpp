#include "JE.hpp"

rack::Plugin* plugin;


void init(rack::Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);

	p->addModel(modelRingModulator);
	p->addModel(modelWaveFolder);

}
