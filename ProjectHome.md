# Description #

This space is about software for different sensors that can be connected to Linux environment.

Currently there is software to get data from Lascar EL-USB-RT (Temp/Hum) and Voltcraft CO-20 (Air Quality) USB sensors. The goal is to make as simple as possible utilities that only collect the value from the USB sensor, so that other software/scripts can use it for processing.

| Sensor | Description|
|:-------|:------------|
|Lascar EL-USB-RT|Lascar EL-USB-RT is an USB dongle that will give both temperature and humidity, more information can be found on their [homepage](http://www.lascarelectronics.com/).|
|Lascar EL-WIFI-T|Lascar EL-WIFI-T is an WIFI based temp sensor that will give data via wireless network. more information can be found on their [homepage](http://www.lascarelectronics.com/)|
|Voltcraft CO-20|The Voltcraft CO-20 Air Quality USB sensor, is a rebranded [Applied Sensors](http://www.appliedsensor.com) product, that exists in many different brands.|

## News ##

25-JAN-2014
  * Started to work on the WIFI sensor (EL-WIFI-T) so it can be accessed on linux, both for setup and receiving data. It will take some time, but good progress so far. It will be based on Python this time.

## Releases ##

[TempSensor Release 0.11 (29-Apr-2013)](http://usb-sensors-linux.googlecode.com/files/TempSensor-0.11.zip)
  * Corrected the calculation of temperature value

## Old ##

Tempsensor Release 0.1 (27-Apr-2013)
  * First release of the Tempsensor utility

## Roadmap ##

For the Tempsensor utility the plans are following:
  * Improve error handling
  * Add repeat function with command switch to choose repeat count

## Other projects ##
  * [RFXCMD](http://code.google.com/p/rfxcmd/), an command line utility to receive and send messages with RFXtrx433 transceiver.
  * [DRControl](http://code.google.com/p/drcontrol/), control Denkovi 4/8 USB Relay board from Linux/MacOSX/Raspberry Pi