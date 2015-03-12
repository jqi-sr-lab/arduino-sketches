# Arduino Sketches

Arduino sketch programs for the JQI Sr lab.

## CurrentController

This is the code running on our [current controllers](https://jqi-wiki.physics.umd.edu/d/documentation/electronics/current_controller).

## DACBoardServer

Code running on the 8-channel DAC box on the rack by the red laser.

## DDSBox

Code running on the 3-channel DDS ["Frequency Breakout" box](https://jqi-wiki.physics.umd.edu/d/documentation/electronics/frequency_breakout_board).

Ideas for updates/improvements:

* Use rotary encoder instead of potentiometer for setpoint

## BeatnoteBox

Code running on the final(ish) beatnote box. Contains:

* 1x PLL clock source (ADF4350)
* 1x BeatnotePLL (v1.1) + associated DDS reference
* 1x auxiliary DDS output (eg, for AOM frequency source)

Also implements frontpanel control (LCD + rotary encoder), and SetList integration via SetListArduino (see `JQIamo/SetListArduino-arduino`).
