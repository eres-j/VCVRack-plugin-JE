#include "JE.hpp"

rack::Plugin* plugin;

void init(rack::Plugin *p)
{
	plugin = p;
	plugin->website = "https://github.com/eres-j/VCVRack-plugin-JE";
	plugin->manual = "https://github.com/eres-j/VCVRack-plugin-JE";

	p->addModel(rack::createModel<RingModulatorWidget>("Ring Modulator"));
	p->addModel(rack::createModel<WaveFolderWidget>("Wave Folder"));
}
