# VCVRack-plugin-JE [![Build Status](https://travis-ci.org/eres-j/VCVRack-plugin-JE.svg?branch=master)](https://travis-ci.org/eres-j/VCVRack-plugin-JE)
Modules for [VCVRack](https://github.com/VCVRack/Rack).

<p align="center">
<img src="docs/screenshots/ring_modulator.png" alt="Ring modulator image">
</p>

## Modules

- Ring Modulator
  - Audio inputs:
    - Modulator -> Attenuation -> Polarity (+/both/-)
    - Carrier -> Attenuation -> Polarity (+/both/-) -> Offset CV -> Offset
  - Audio outputs:
    - Ring modulation
    - Sum = Carrier + Modulator
    - Diff = Carrier - Modulator
    - Min = Sum < Diff ? Sum : Diff
    - Max = Sum > Diff ? Sum : Diff
  - Diode characteristics:
    - Vb = Forward bias voltage
    - Vl = Voltage beyond which the function is linear
    - Slope = Slope of the linear section

## Building from sources

Clone this repository into the Rack's plugins directory then build using Make:

``` bash
cd Rack/plugins/
git clone https://github.com/eres-j/VCVRack-plugin-JE.git
cd VCVRack-plugin-JE
make -j
```
