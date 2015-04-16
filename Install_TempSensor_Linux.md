# Introduction #

Instructions how to install the 'tempsensor' utility on Linux.

# Tested on #

  * Debian 6 i386 (via VMWare)
  * Ubuntu 13.04 64bit (via VMWare)
  * Raspbian (Raspberry Pi)

# Details #

## Requirements ##

  * The libusb 1.0 is required for the USB communication.
  * The Lascar EL-USB-RT temperature/humidity dongle

## LibUSB 1.0 ##

Install LibUSB 1.0

```
$ sudo apt-get install libusb-1.0
```


---

# Compile tempsensor from sources #

## Install required software ##

The 'build-essential' package is needed to be able to compile the source code.

```
$ sudo apt-get install build-essential
```

## Download the source code and compile ##

Download the latest released version from GoogleCode.

```
$ mkdir tempsensor
$ cd tempsensor
$ wget http://usb-sensors-linux.googlecode.com/files/TempSensor-0.11.zip
$ unzip TempSensor-0.11.zip
```

Compile the source code

```
$ gcc -o tempsensor tempsensor.c -lusb-1.0
```

## Test ##

You can now execute the tempsensor and see if you get readings, as seen on most Linux installations it seems that you must be root user, or then add udev rule to make the device read/write.

To run with root access

```
$ sudo ./tempsensor 
Temperature: 16.2C, Humidity: 22%
$
```

Without root access it will print following

```
$ ./tempsensor 
Error: Unable to find the device.
Error: Can not init USB device
$
```

## Make Udev rule ##

Insert a udev rule to get read/write access to the device from normal user.

Create and edit following file:

```
$ sudo nano /etc/udev/rules.d/99-usb.rules
```

Add following lines;

```
# Lascar 
SUBSYSTEM=="usb", ATTR{idVendor}=="1781", ATTR{idProduct}=="0ec4", MODE="0666"
```

Save and reboot, now the device should be writable and give you results.

Example

```
$ ./tempsensor 
Temperature: 24.5C, Humidity: 22%
$
```

Please see [readme](ReadMe_TempSensor.md) for more usage instructions


---

# Troubleshooting #

## Permission denied ##

If you get following issue when running tempsensor:

```
$ ./tempsensor 
libusb couldn't open USB device /dev/bus/usb/002/005: Permission denied.
```

It means that you have no access to the USB device, two solutions;

a) use sudo, eg "sudo ./tempsensor"

b) create the udev rule as above