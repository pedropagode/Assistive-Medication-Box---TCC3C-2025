Project Description – Assistive Medicine Box

This Final Course Project presents the development of an Intelligent Assistive Medicine Box, designed to support users in the correct management of medication schedules, reducing forgetfulness, delays, and administration errors. The system integrates embedded systems, automation, and low-latency wireless communication, ensuring reliability and fast response to alerts.

The proposed solution adopts a distributed architecture composed of three main microcontrollers: Arduino Mega, ESP8266, and ESP32-C3 Mini, each responsible for specific tasks within the system. Wireless data transmission between the ESP8266 and the ESP32-C3 Mini is performed using the ESP-NOW protocol, enabling direct device-to-device communication without relying on external network infrastructure.

The system architecture is divided into three functional layers:

* Logical Control Layer – Arduino Mega
* Wireless Communication Layer – ESP8266
* User Interface and Actuation Layer – ESP32-C3 Mini

Communication between the ESP8266 and the ESP32-C3 Mini is established through ESP-NOW, a proprietary protocol developed by Espressif that allows fast, reliable, and low-power data exchange without the need for Wi-Fi routers or internet connectivity. This approach is particularly suitable for real-time and assistive embedded systems.

System Components:

* Arduino Mega – Central Control Unit

The Arduino Mega is responsible for the core logic of the Assistive Medicine Box, performing tasks such as:
Registration and storage of medication schedules;
Continuous comparison between the current time and programmed alarms;
Management of alarm states (active, triggered, acknowledged);
Reading user input devices, such as buttons and sensors;
Sending system states and commands to the ESP8266 via serial communication.
The Arduino Mega was selected due to its large number of digital and analog input/output pins, which allows the integration of multiple peripherals simultaneously.

* ESP8266 – Wireless Communication Module

The ESP8266 acts as the wireless communication module of the system. It receives processed data from the Arduino Mega via serial communication, including:
Alarm identifiers;
System status information;
Activation or deactivation commands.
After receiving these data, the ESP8266 encapsulates the information into structured packets and transmits them to the ESP32-C3 Mini using the ESP-NOW protocol, ensuring:
Low communication latency;
High reliability;
Independence from external Wi-Fi networks;
Reduced power consumption.

* ESP32-C3 Mini – User Interface and Actuation Unit

The ESP32-C3 Mini is responsible for direct interaction with the user and for executing the physical actions of the medicine box. Its main functions include:
Activation of visual indicators, such as LEDs;
Triggering of audible alerts (buzzers) and/or haptic feedback (vibration motors);
Control of displays or visual indicators, when available;
Reception of ESP-NOW packets sent by the ESP8266;
Transmission of user acknowledgment messages back to the ESP8266.
Once the user confirms medication intake, the ESP32-C3 Mini sends a response via ESP-NOW, allowing the system to properly update the alarm status.

* Data Transmission Logic Using ESP-NOW
The user configures medication schedules in the Assistive Medicine Box;
The Arduino Mega stores the data and continuously monitors the current time;
When a scheduled time is reached, the Arduino Mega detects the alarm trigger;
The event is sent to the ESP8266 through serial communication;
The ESP8266 transmits the alarm data to the ESP32-C3 Mini using ESP-NOW;
The ESP32-C3 Mini immediately activates visual, audible, and/or haptic alerts;
The user interacts with the system and confirms medication intake;
The confirmation message is sent back to the ESP8266 via ESP-NOW;
The ESP8266 forwards the updated status to the Arduino Mega, which records the event and closes the alarm cycle.

Technical Considerations on ESP-NOW Usage

The adoption of the ESP-NOW protocol in this project offers several advantages:

* Direct peer-to-peer communication;
* No dependency on routers or internet connectivity;
* Fast response times suitable for critical alerts;
* High reliability in short-range environments;
* Ideal for assistive and real-time embedded applications.

These characteristics make ESP-NOW an appropriate and efficient solution for the Assistive Medicine Box.

Here is the link to the video demonstrating how the project works.
https://youtu.be/DHLaOhzLl80
