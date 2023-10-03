<a href="https://www.microchip.com" rel="nofollow"><img src="images/microchip.png" alt="MCHP" width="300"/></a>

# AN3408 - How to Use Force Sensitive Resistor with a 12-bit ADC

This application note describes how to use the Analog-to-digital converter (ADC) on the tinyAVR® 2 Family microcontrollers to measure a Force Sensitive Resistor. The ADC supports sampling in bursts where a configurable number of conversion results are accumulated into a single ADC result (sample accumulation). This feature is used in the example application to accumulate 1024 samples and perform averaging.

In the example application, the ADC result will be sent to an android/iOS app over Bluetooth communication using the RN4870 Click board. 
The ADC result will also be sent to a serial terminal and a 4x4 RGB Click board™ is used to illuminate different LEDs corresponding to the applied force of the force sensor on the Force Click board.

## Related Documentation

- [AN3408 - How to use Force Sensitive Resistor with 12-bit ADC](https://microchip.com/DS00003408)
- [ATtiny1627 device page](https://www.microchip.com/wwwproducts/en/ATTINY1627)

## Software Used

- [Microchip Studio for AVR® and SAM Devices 7.0.2594](https://www.microchip.com/en-us/development-tools-tools-and-software/microchip-studio-for-avr-and-sam-devices) or newer
- [Microchip AVR64EA48 Device Support Pack ATtiny_DFP v2.0.368](https://packs.download.microchip.com/) or newer
- [AVR® GCC 5.4.0 compiler](https://www.microchip.com/en-us/development-tools-tools-and-software/gcc-compilers-avr-and-arm) or newer  

## Setup

- Connect the ATtiny1627 Curiosity Nano on Curiosity Nano Adapter Board
- Connect RN4870 Click on the Slot1 on Curiosity Nano Adapter Board
- Connect Force Click  on the Slot2 on Curiosity Nano Adapter Board
- Connect 4x4 RGB Click on the Slot3 on Curiosity Nano Adapter Board

![Connection Diagram](images/hw_setup.svg "Connection Diagram")

# Operation

- Connect the ATtiny1627 Curiosity Nano to a computer using a USB cable
- Download the application and program it to the ATtiny1627 Curiosity Nano
- Refer to topic 'Demo Operation' in the Application Note

## Conclusion

This example has shown how to measure a Force Sensitive Resistor using the ADC on a tinyAVR 2 Family microcontroller, and shows the results sent over both LEDs and Bluetooth.  
