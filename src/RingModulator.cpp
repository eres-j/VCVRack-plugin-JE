#include "JE.hpp"
#include "common/constants.hpp"
#include "component/diode.hpp"

// STL
#include <cmath>
#include <atomic>
#include <iostream>

/*
http://recherche.ircam.fr/pub/dafx11/Papers/66_e.pdf
TODO:
Interpolate lookup table
Threading on Vb, Vl, H changes
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

	inline static float clampToAudioVoltage(const float v)
	{
		return rack::clampf(v, -g_audioPeakVoltage, g_audioPeakVoltage);
	}

	inline static float clampToControlVoltage(const float v)
	{
		return rack::clampf(v, -g_controlPeakVoltage, g_controlPeakVoltage);
	}

	inline void tickTime()
	{
		m_time += 1.f / 44100.f;
	}

	inline float vco_sin(float f) const
	{
		return sin(2.f * 3.1415f * f * m_time);
	}

	inline float getParameterValue(const ParamIds id) const
	{
		return params[id].value;
	}

	inline float getAudioInputValue(const InputIds id) const
	{
		/*if (id == INPUT_INPUT)
			return vco_sin(50.f) * 10.f;
		else if (id == CARRIER_INPUT)
			return vco_sin(150.f) * 10.f;*/
		return clampToAudioVoltage(inputs[id].value);
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
		tickTime();

		if (!needToStep())
			return;

		updateDiodeCharacteristics();

		const float vhin = getLeveledPolarizedInputValue(INPUT_INPUT, INPUT_POLARITY_PARAM, INPUT_LEVEL_PARAM) * 0.5f;
		const float vc = clampToAudioVoltage(getLeveledPolarizedInputValue(CARRIER_INPUT, CARRIER_POLARITY_PARAM, CARRIER_LEVEL_PARAM) +
			clampToControlVoltage(getParameterValue(CARRIER_OFFSET_PARAM) + getControlInputValue(CARRIER_OFFSET_INPUT)));

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

	float m_time = 0.f;
};

RingModulatorWidget::RingModulatorWidget()
{
	RingModulator* module = new RingModulator();
	setModule(module);
	box.size = rack::Vec(15*8, 380);

	addChild(rack::createScrew<rack::ScrewSilver>(rack::Vec(15, 0)));
	addChild(rack::createScrew<rack::ScrewSilver>(rack::Vec(box.size.x-30, 0)));
	addChild(rack::createScrew<rack::ScrewSilver>(rack::Vec(15, 365)));
	addChild(rack::createScrew<rack::ScrewSilver>(rack::Vec(box.size.x-30, 365)));

	float yOffset = 67.f;

	float y = 57.f;
	addParam(rack::createParam<rack::RoundBlackKnob>(rack::Vec(62, y), module, RingModulator::INPUT_LEVEL_PARAM, 0.0, 1.0, 1.0));
	y += yOffset;
	addParam(rack::createParam<rack::RoundBlackKnob>(rack::Vec(62, y), module, RingModulator::CARRIER_LEVEL_PARAM, 0.0, 1.0, 1.0));
	y += yOffset;
	addParam(rack::createParam<rack::RoundBlackKnob>(rack::Vec(62, y), module, RingModulator::CARRIER_OFFSET_PARAM, -g_controlPeakVoltage, g_controlPeakVoltage, 0.0));
	y += yOffset;
	addParam(rack::createParam<rack::NKK>(rack::Vec(9, y), module, RingModulator::INPUT_POLARITY_PARAM, 0.0, 2.0, 2.0));
	y += yOffset;
	addParam(rack::createParam<rack::NKK>(rack::Vec(9, y), module, RingModulator::CARRIER_POLARITY_PARAM, 0.0, 2.0, 2.0));
	y -= yOffset;
	addParam(rack::createParam<rack::RoundSmallBlackKnob>(rack::Vec(62, y), module, RingModulator::DIODE_VB_PARAM, std::numeric_limits<float>::epsilon(), g_controlPeakVoltage, 0.2));
	y += yOffset / 2.f;
	addParam(rack::createParam<rack::RoundSmallBlackKnob>(rack::Vec(62, y), module, RingModulator::DIODE_VL_MINUS_VB_PARAM, std::numeric_limits<float>::epsilon(), g_controlPeakVoltage, 0.5));
	y += yOffset / 2.f;
	addParam(rack::createParam<rack::RoundSmallBlackKnob>(rack::Vec(62, y), module, RingModulator::DIODE_H_PARAM, 0.0, 1.0, 0.9));

	y = 63.f;
	addInput(rack::createInput<rack::PJ301MPort>(rack::Vec(9, y), module, RingModulator::INPUT_INPUT));
	y += yOffset;
	addInput(rack::createInput<rack::PJ301MPort>(rack::Vec(9, y), module, RingModulator::CARRIER_INPUT));
	y += yOffset;
	addInput(rack::createInput<rack::PJ301MPort>(rack::Vec(9, y), module, RingModulator::CARRIER_OFFSET_INPUT));

	yOffset = 32.f;
	y = 224.f;
	addOutput(rack::createOutput<rack::PJ301MPort>(rack::Vec(87, y), module, RingModulator::RING_OUTPUT));
	y += yOffset;
	addOutput(rack::createOutput<rack::PJ301MPort>(rack::Vec(87, y), module, RingModulator::SUM_OUTPUT));
	y += yOffset;
	addOutput(rack::createOutput<rack::PJ301MPort>(rack::Vec(87, y), module, RingModulator::DIFF_OUTPUT));
	y += yOffset;
	addOutput(rack::createOutput<rack::PJ301MPort>(rack::Vec(87, y), module, RingModulator::MIN_OUTPUT));
	y += yOffset;
	addOutput(rack::createOutput<rack::PJ301MPort>(rack::Vec(87, y), module, RingModulator::MAX_OUTPUT));
}
