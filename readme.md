<a href="https://microchip.com" target="_blank" align="left">
  <img width=200px height=auto src="graphics/MicrochipLogo.png">
</a>


# **How to use Force Sensitive Resistor with 12-bit ADC**


## **Introduction**

This application note describes how to use the ADC on tinyAVR® 2 Family microcontrollers to measure a Force Sensitive Resistor. The ADC supports sampling in bursts where a configurable number of conversion results are accumulated into a single ADC result (sample accumulation). This feature is used in the example application to accumulate 1024 samples and perform averaging.

In the example application, the ADC result will be sent to an android/iOS app over Bluetooth communication using the RN4870 Click board. 
The ADC result will also be sent to a serial terminal and a 4x4 RGB Click board is used to illuminate different LEDs corresponding to the applied force on the force sensor on the Force Click board.


## **Relevant Documents**
The application note explaining the concepts used in this repository can be found at https://microchip.com/DS00003408


## **Running the Example**

In the example application, the ADC result will be sent to an android/iOS app over Bluetooth communication using the RN4870 Click board. The ADC result will also be sent to a serial terminal and a 4x4 RGB Click board is used to illuminate different LEDs corresponding to the applied force on the force sensor on the Force Click board.

- Connect the ATtiny1627 Curiosity Nano on Curiosity Nano Adapter Board
- Connect RN4870 Click on the Slot1 on Curiosity nano Adapter Board
- Connect Force Click  on the Slot2 on Curiosity nano Adapter Board
- Connect 4x4 RGB Click on the Slot3 on Curiosity nano Adapter Board
- Connect the ATtiny1627 Curiosity Nano to a computer using a USB cable
- Download the application and program it to the ATtiny1627 Curiosity Nano
- Refer to topic 'Demo Operation' in Application Note
<p align="left">
  <img width=800px height=auto src="graphics/hw_setup.svg">
  <br>Hardware Setup<br>
</p>