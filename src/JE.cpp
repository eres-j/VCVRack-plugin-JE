#include "JE.hpp"

rack::Plugin* pluginInstance;

void init(rack::Plugin *p)
{
    pluginInstance = p;

	p->addModel(modelRM);
	p->addModel(modelWF);
}
