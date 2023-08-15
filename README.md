# Lift Pump - Summary of Operation

When I bought my house, there was an original pump, but after many breakdowns and having noticed the presence of mud and H2S in large quantities, I embarked on this project. The cable sheathing was being eaten away by the H2S and the pump body, which was not submerged, was very badly attacked by rust.

 I switched to this esp32 management system, as I wasn't satisfied with what was available. Too expensive, no configuration possible. It was useful once when hair started to surround the rotating joint of the stator, I was able to repair before it was too late, and it's never glamorous this kind of unexpected failure. and it's very instructive this kind of project. I have an arduino in production with an RPI for the wifi part, I'm going to switch everything over to an ESP32, here's my project. I'm not a programmer by trade, so forgive my mistakes and monstrosities.

This project consists of creating a control system for a lift pump. The aim is to monitor the operation of the pump, record the actual operating times, the intervals between starts and display this information in tabular form via a web interface. and also :
- limit the presence of H2S gas and odours
- make full use of tank capacity
- protect the pump well in advance from existing problems, hair on the rotor, blocked rotor, blocked sensor and avoid burning out the motor.

## Features

- Automatic pump start-up when the high level sensor is activated.
- Automatic pump shutdown after a certain operating time.
- Recording of actual running times and intervals between starts.
- Web interface for displaying the latest pump operations.
- Safety device to detect any blockage of the high level sensor.

## Components used

- Microcontroller: ESP32
- High level sensor: Stainless steel float switch, liquid water level sensor, double ball float switch, reed switch.
- Safety relay: Relay to cut power to the pump if necessary, type :
    K8DT-AW2CD Current monitoring relay
Omron

RM35JA32MW Current monitoring relay, 
schneider electrique

GRI8-03/04 On current or under current
GEYA
- Web interface: Use of AsyncWebServer to display the table of the last operations, in AP mode and possibility of configuring the pumping times with a telephone.

## Operation

1. The ESP32 microcontroller continuously monitors the status of the high level sensor.
2. When the high level sensor is activated, the pump starts automatically.
3. The pump runs for a set time (e.g. 10 seconds) before stopping automatically. to be configured in the web interface
4. Actual running times and intervals between starts are recorded in tables. accessible by telephone
5. A web interface allows the last pump operations to be displayed in table form.
6. If the high level sensor is blocked after start-up, the safety relay is activated to cut power to the pump.
until reset by the user via the web interface.
7. connect to the pompe_relevage wifi, enter the name: pomperelevage in the address bar to access the site.

## Configuration

- Make sure you configure the appropriate constants in the code, such as operating times and pins used.
- Make sure you have the necessary libraries, such as WiFi, DNSServer and AsyncWebServer. to be able to compile
- I have chosen to put everything in a single file to simplify compilation for novices with the arduino-1.8.19 gui.

## Note

This project is designed for educational and experimental purposes. Make sure you take all the necessary precautions when handling electrical equipment.
