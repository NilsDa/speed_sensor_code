# speed_sensor_code

contains segger embedded studio project and certain needed files/drivers for the speed_sensor firmware. Additionally to this repo, to build the project, the nordic nrf5 SDK 17.0.2 aaand the softdevice 212 files obtained from thisisant are needed.

## Status:

- all needed timers and interrupts work well and functionality is tested
- communication with acceleration-sensor is tested
- function to calculate rotation rate (and therefore bicycle speed) needs to be finalized and tested
  - testing can be done by recording data onto SDcard (see corresponding branch) or by getting ANT+ to work first and then test with live data
    - sensible option would be to record data. -> SPI connection to SDcard is tested and works. However needs to be resoldered and soldering is difficult and mechanically unstable because of poor PCB design (shame on me...).
- antenna PCB-layout is tested and works well (at least for a BLE example-implementation)
- ANT+ functionality could not yet be tested... because my Phone is buggy and the ANT+ service plugin doesn't work and I don't have an ANT+ dongle (to use my Laptop for testing)
