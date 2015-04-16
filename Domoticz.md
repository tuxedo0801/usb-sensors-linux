# Introduction #

Instructions how to send sensor data to the Domoticz application. Domoticz is a home automation system that can monitor different sensors via RFXtrx433 device, but it is also possible to send data via the web interface.

The only thing that is needed is a small bash script that collect the data, and send it to Domoticz with the curl utility.




---

# Send data from Tempsensor (Lascar EL-USB-RT) #

It is assumed that the tempsensor utility is installed and is working properly.

## Script ##

The bash script file, we call it send2domoticz.sh

```
#!/bin/sh

# Domoticz server
SERVER="a.b.c.d:8080"

# Get data
INDATA=`/home/user/tempsensor/tempsensor -s`
TEMP=`echo $INDATA | cut -d \, -f 1`
HUM=`echo $INDATA | cut -d \, -f 2`

# If TEMP/HUM contains value then send it to Domoticz
if [ -z "$TEMP" -a -z "$HUM" ];
then
	echo "Error: Value variable empty"
else
        # Sensor parameters
        HID="1"
        DID="4000"
        DUNIT="4"
        DTYPE="82"
        DSUBTYPE="1"
        NVALUE="0"
        SVALUE="$TEMP;$HUM;9"

        # Send data
        curl -s -i -H "Accept: application/json" "http://$SERVER/json.htm?type=command&param=udevice&hid=$HID&did=$DID&dunit=$DUNIT&dtype=$DTYPE&dsubtype=$DSUBTYPE&nvalue=$NVALUE&svalue=$SVALUE"
fi
```

Following lines needs to be edited to suit your system

  * Variable _**SERVER**_ this should be the address for your Domoticz server
  * Variable _**INDATA**_ should contain the path where the Tempsensor utility is located

With these sensor type the data will be reported as type "Temp + Humidity" and subtype "Oregon THGN122/123, THGN132, THGR122/228/238/268".

## Schedule ##

To make it automatic, you need to add the script to the crontab.

To edit crontab with following command

```
$ crontab -e
```

And add following line (note, to change the path to your correct path)

```
*/1 * * * *     /home/user/tempsensor/send2domoticz.sh
```

Now the data should go to Domoticz every minute.

## Example ##
Example of a 24 hour sensor data in Domoticz from Lascar EL-USB-RT:

![http://usb-sensors-linux.googlecode.com/svn/images/tempsensor_domoticz.png](http://usb-sensors-linux.googlecode.com/svn/images/tempsensor_domoticz.png)


---

# Send data from Airsensor (Indoor Air Quality USB Sensor) #

Note, since Domoticz does not know this sensor natively, we need to mask it as an temperature sensor that RFXCOM is supporting, this mean that the value will be correct at all times, but it will be shown as a temperature sensor, and the scale will be in celsius degrees.

It is assumed that the tempsensor utility is installed and is working properly.

## Script ##

The bash script file, we call it send2domoticz.sh

```
#!/bin/bash

SERVER="a.b.c.d:8080"

# Get temperature
VOC=`/home/user/airsensor/airsensor -v -o |tr -d '\040\010\012\015'`

# If TEMP contains value then send it to Domoticz
if [[ ! "$VOC" -eq "0" ]]
then

	# Sensor parameters
	HID="1"
	DID="2000"
	DUNIT="3"
	DTYPE="80"
	DSUBTYPE="1"
	NVALUE="0"
	SVALUE="$VOC"

	# Send data
	curl -s -i -H "Accept: application/json" "http://$SERVER/json.htm?type=command&param=udevice&hid=$HID&did=$DID&dunit=$DUNIT&dtype=$DTYPE&dsubtype=$DSUBTYPE&nvalue=$NVALUE&svalue=$SVALUE"

fi
```

Following lines needs to be edited to suit your system

  * Variable _**SERVER**_ this should be the address for your Domoticz server
  * Variable _**VOC**_ should contain the path where the Airsensor utility is located

With these sensor type the data will be reported as type "Temp" and subtype "Oregon THR128/138, THC138".

You need to give execute access to the script.

```
$ chmod +x send2domoticz.sh
```

Then you can test manually if the script works

```
$ ./send2domoticz.sh
```

Example:
```
$ ./send2domoticz.sh 
HTTP/1.0 200 OK
Content-Length: 53
Content-Type: text/html;charset=ISO-8859-1
Cache-Control: no-cache
Pragma: no-cache

{
   "status" : "OK",
   "title" : "Update Device"
}
$
```

The Domoticz will answer with status OK, and the device should be visible in Domoticz device menu.

## Schedule ##

To make it automatic, you need to add the script to the crontab.

To edit crontab with following command

```
$ crontab -e
```

And add following line (note, to change the path to your correct path)

```
*/1 * * * *     /home/user/airsensor/send2domoticz.sh
```

Now the data should go to Domoticz every minute.

## Example ##