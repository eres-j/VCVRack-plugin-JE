#include "JE.hpp"
#include "common/constants.hpp"
#include "component/diode.hpp"

// STL
#include <cmath>

/*
http://recherche.ircam.fr/pub/dafx11/Papers/66_e.pdf
*/

struct RingModulator : rack::Module
{
	enum ParamIds
	{
		INPUT_LEVEL_PARAM = 0,
		CARRIER_LEVEL_PARAM,
		CARRIER_OFFSET_PARAM,
		INPUT_POLARITY_PARAM,
		CARRIER_POLARITY_PARAM,
		DIODE_VB_PARAM,
		DIODE_VL_MINUS_VB_PARAM,
		DIODE_H_PARAM,

		NUM_PARAMS
	};

	enum InputIds
	{
		INPUT_INPUT = 0,
		CARRIER_INPUT,
		CARRIER_OFFSET_INPUT,

		NUM_INPUTS
	};

	enum OutputIds
	{
		RING_OUTPUT = 0,
		SUM_OUTPUT,
		DIFF_OUTPUT,
		MIN_OUTPUT,
		MAX_OUTPUT,

		NUM_OUTPUTS
	};

	enum LightIds
	{
		NUM_LIGHTS = 0
	};

	RingModulator()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	{}

	inline bool needToStep()
	{
		if (!outputs[RING_OUTPUT].active &&
			!outputs[SUM_OUTPUT].active &&
			!outputs[DIFF_OUTPUT].active &&
			!outputs[MIN_OUTPUT].active &&
			!outputs[MAX_OUTPUT].active)
			return false;

		if (!inputs[INPUT_INPUT].active || !inputs[CARRIER_INPUT].active)
		{
			outputs[RING_OUTPUT].value = 0.f;
			return false;
		}

		return true;
	}

	inline void updateDiodeCharacteristics()
	{
		m_diode.setVb(getParameterValue(DIODE_VB_PARAM));
		m_diode.setVlMinusVb(getParameterValue(DIODE_VL_MINUS_VB_PARAM));
		m_diode.setH(getParameterValue(DIODE_H_PARAM));
	}

	inline float getParameterValue(const ParamIds id) const
	{
		return params[id].value;
	}

	inline float getAudioInputValue(const InputIds id) const
	{
		return inputs[id].value;
	}

	inline float getControlInputValue(const InputIds id) const
	{
		return inputs[id].value;
	}

	inline void setOutputValue(OutputIds id, float v)
	{
		outputs[id].value = v;
	}

	inline float getLeveledPolarizedInputValue(InputIds inId, ParamIds polarityPrmId, ParamIds levelPrmId)
	{
		const float inputPolarity = getParameterValue(polarityPrmId);
		const float inputValue = getAudioInputValue(inId) * getParameterValue(levelPrmId);

		if (inputPolarity < 0.5f)
			return (inputValue < 0.0f) ? -m_diode(inputValue) : 0.f;
		else if (inputPolarity > 1.5f)
			return (inputValue > 0.0f) ? m_diode(inputValue) : 0.f;
		return inputValue;
	}

	inline void step() override
	{
		if (!needToStep())
			return;

		updateDiodeCharacteristics();

		const float vhin = getLeveledPolarizedInputValue(INPUT_INPUT, INPUT_POLARITY_PARAM, INPUT_LEVEL_PARAM) * 0.5f;
		const float vc = getLeveledPolarizedInputValue(CARRIER_INPUT, CARRIER_POLARITY_PARAM, CARRIER_LEVEL_PARAM) +
			getParameterValue(CARRIER_OFFSET_PARAM) + getControlInputValue(CARRIER_OFFSET_INPUT);

		const float vc_plus_vhin = vc + vhin;
		const float vc_minus_vhin = vc - vhin;

		const float sum = m_diode(vc_plus_vhin);
		setOutputValue(SUM_OUTPUT, sum);

		const float diff = m_diode(vc_minus_vhin);
		setOutputValue(DIFF_OUTPUT, diff);

		setOutputValue(MIN_OUTPUT, sum < diff ? sum : diff);
		setOutputValue(MAX_OUTPUT, sum > diff ? sum : diff);

		const float out = sum - diff;
		setOutputValue(RING_OUTPUT, out);
	}

private:
	Diode m_diode;
};

struct RingModulatorWidget : rack::ModuleWidget
{
	RingModulatorWidget(RingModulator *module);
};

RingModulatorWidget::RingModulatorWidget(RingModulator *module) : ModuleWidget(module) {
/*RingModulatorWidget::RingModulatorWidget()
{
	RingModulator* module = new RingModulator();
	setModule(module);*/
	box.size = rack::Vec(15*10, 380);

	{
		rack::SVGPanel *panel = new rack::SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(rack::SVG::load(assetPlugin(plugin, "res/CleanRingModulator.svg")));
		addChild(panel);
	}

	addChild(rack::Widget::create<rack::ScrewBlack>(rack::Vec(15, 0)));
	addChild(rack::Widget::create<rack::ScrewBlack>(rack::Vec(box.size.x-30, 0)));
	addChild(rack::Widget::create<rack::ScrewBlack>(rack::Vec(15, 365)));
	addChild(rack::Widget::create<rack::ScrewBlack>(rack::Vec(box.size.x-30, 365)));

	const float switchX = box.size.x - 40.f;

	float yOffset = 67.f;

	float portY = 63.f;
	float knobY = 57.f;
	float switchY = 54.f;
	addInput(rack::Port::create<rack::PJ301MPort>(rack::Vec(9, portY), Port::INPUT, module, RingModulator::INPUT_INPUT));
	addParam(rack::ParamWidget::create<rack::RoundBlackKnob>(rack::Vec(54, knobY), module, RingModulator::INPUT_LEVEL_PARAM, 0.0, 1.0, 1.0));
	addParam(rack::ParamWidget::create<rack::NKK>(rack::Vec(switchX, switchY), module, RingModulator::INPUT_POLARITY_PARAM, 0.0, 2.0, 1.0));

	portY += yOffset;
	knobY += yOffset;
	switchY += yOffset;
	addInput(rack::Port::create<rack::PJ301MPort>(rack::Vec(9, portY), Port::INPUT, module, RingModulator::CARRIER_INPUT));
	addParam(rack::ParamWidget::create<rack::RoundBlackKnob>(rack::Vec(54, knobY), module, RingModulator::CARRIER_LEVEL_PARAM, 0.0, 1.0, 1.0));
	addParam(rack::ParamWidget::create<rack::NKK>(rack::Vec(switchX, switchY), module, RingModulator::CARRIER_POLARITY_PARAM, 0.0, 2.0, 1.0));

	portY += yOffset;
	knobY += yOffset;
	switchY += yOffset;
	addInput(rack::Port::create<rack::PJ301MPort>(rack::Vec(9, portY), Port::INPUT, module, RingModulator::CARRIER_OFFSET_INPUT));
	addParam(rack::ParamWidget::create<rack::RoundBlackKnob>(rack::Vec(54, knobY), module, RingModulator::CARRIER_OFFSET_PARAM, -g_controlPeakVoltage, g_controlPeakVoltage, 0.0));
	addOutput(rack::Port::create<rack::PJ301MPort>(rack::Vec(box.size.x-34, portY), Port::OUTPUT, module, RingModulator::RING_OUTPUT));

	portY += yOffset;
	knobY += yOffset;
	switchY += yOffset;
	float xOffset = 35.f;
	float x = 9.f;
	addOutput(rack::Port::create<rack::PJ301MPort>(rack::Vec(x, portY), Port::OUTPUT, module, RingModulator::SUM_OUTPUT));
	x += xOffset;
	addOutput(rack::Port::create<rack::PJ301MPort>(rack::Vec(x, portY), Port::OUTPUT, module, RingModulator::DIFF_OUTPUT));
	x += xOffset;
	addOutput(rack::Port::create<rack::PJ301MPort>(rack::Vec(x, portY), Port::OUTPUT, module, RingModulator::MIN_OUTPUT));
	x += xOffset;
	addOutput(rack::Port::create<rack::PJ301MPort>(rack::Vec(x, portY), Port::OUTPUT, module, RingModulator::MAX_OUTPUT));

	portY += yOffset;
	knobY += yOffset;
	switchY += yOffset;
	const float y = knobY - 6.f;
	xOffset = 52.f;
	x = 9.f;
	addParam(rack::ParamWidget::create<rack::RoundSmallBlackKnob>(rack::Vec(x, y), module, RingModulator::DIODE_VB_PARAM, std::numeric_limits<float>::epsilon(), g_controlPeakVoltage, 0.2));
	x += xOffset;
	addParam(rack::ParamWidget::create<rack::RoundSmallBlackKnob>(rack::Vec(x, y), module, RingModulator::DIODE_VL_MINUS_VB_PARAM, std::numeric_limits<float>::epsilon(), g_controlPeakVoltage, 0.5));
	x += xOffset;
	addParam(rack::ParamWidget::create<rack::RoundSmallBlackKnob>(rack::Vec(x, y), module, RingModulator::DIODE_H_PARAM, 0.0, 1.0, 0.9));
}

Model *modelRingModulator = Model::create<RingModulator, RingModulatorWidget>(
/*p->addModel(rack::createModel<RingModulatorWidget>(*/
	TOSTRING(SLUG), "RingModulator", "Ring Modulator", EFFECT_TAG, RING_MODULATOR_TAG);