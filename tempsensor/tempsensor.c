/*
 * tempsensor.c
 *
 * Based on generic_hid.c by Jan Axelson (Apr 22, 2011)
 *
 *
The application uses the libusb 1.0 API from libusb.org.
Compile the application with the -lusb-1.0 option. 
Use the -I option if needed to specify the path to the libusb.h header file. For example:
-I/usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/include/libusb-1.0 

 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/libusb-1.0/libusb.h>

// Values for bmRequestType in the Setup transaction's Data packet.
static const int CONTROL_REQUEST_TYPE_IN = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;
static const int CONTROL_REQUEST_TYPE_OUT = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE;

// From the HID spec:
static const int HID_GET_REPORT = 0x01;
static const int HID_SET_REPORT = 0x09;
static const int HID_REPORT_TYPE_INPUT = 0x01;
static const int HID_REPORT_TYPE_OUTPUT = 0x02;
static const int HID_REPORT_TYPE_FEATURE = 0x03;

// With firmware support, transfers can be > the endpoint's max packet size.
static const int MAX_CONTROL_IN_TRANSFER_SIZE = 3;
static const int MAX_CONTROL_OUT_TRANSFER_SIZE = 3;

static const int INTERFACE_NUMBER = 0;
static const int TIMEOUT_MS = 5000;

int read_usb(libusb_device_handle *devh);

// Use interrupt transfers to to write data to the device and receive data from the device.
// Returns - zero on success, libusb error code on failure.
int read_usb(libusb_device_handle *devh)
{
	
	// Assumes interrupt endpoint 2 IN and OUT:
	static const int INTERRUPT_IN_ENDPOINT = 0x81;
	static const int INTERRUPT_OUT_ENDPOINT = 0x01;
	
	// With firmware support, transfers can be > the endpoint's max packet size.
	static const int MAX_INTERRUPT_IN_TRANSFER_SIZE = 5;
	static const int MAX_INTERRUPT_OUT_TRANSFER_SIZE = 5;
	
	int bytes_transferred;
	int i = 0;
	int result = 0;;
	
 	char data_in[MAX_INTERRUPT_IN_TRANSFER_SIZE];
	char data_out[MAX_INTERRUPT_OUT_TRANSFER_SIZE];
	
	// Read data from the device.
	result = libusb_interrupt_transfer(
			devh,
			INTERRUPT_IN_ENDPOINT,
			data_in,
			MAX_INTERRUPT_OUT_TRANSFER_SIZE,
			&bytes_transferred,
			TIMEOUT_MS);
	
	if (result >= 0)
	{
		if (bytes_transferred > 0)
		{
			printf("Data received via interrupt transfer:\n");
			for(i = 0; i < bytes_transferred; i++) {
				printf("%02x ",data_in[i]);
			}
			printf("\n");
		} else {
			fprintf(stderr, "No data received in interrupt transfer (%d)\n", result);
			return -1;
		}
	} else {
		fprintf(stderr, "Error receiving data via interrupt transfer %d\n", result);
		return result;
	}
	
	return 0;
}

int main(void)
{
	static const int VENDOR_ID = 0x1781;
	static const int PRODUCT_ID = 0x0ec4;
	
	struct libusb_device_handle *devh = NULL;
	int device_ready = 0;
	int result;
	
	result = libusb_init(NULL);
	if (result >= 0)
	{
		devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
		
		if (devh != NULL)
		{
			// The HID has been detected.
			// Detach the hidusb driver from the HID to enable using libusb.
			
			libusb_detach_kernel_driver(devh, INTERFACE_NUMBER);
			{
				result = libusb_claim_interface(devh, INTERFACE_NUMBER);
				if (result >= 0)
				{
					device_ready = 1;
					fprintf(stdout, "USB Device ready\n");
				}
				else
				{
					fprintf(stderr, "libusb_claim_interface error %d\n", result);
				}
			}
		}
		else
		{
			fprintf(stderr, "Unable to find the device.\n");
		}
	}
	else
	{
		fprintf(stderr, "Unable to initialize libusb.\n");
	}
	
	if (device_ready)
	{
		// Send and receive data.
		read_usb(devh);
		
		// Finished using the device.
		fprintf(stdout, "Release interface\n");
		libusb_release_interface(devh, 0);
	}
	
	fprintf(stdout, "Close USB Device\n");
	libusb_close(devh);
	libusb_exit(NULL);
	return 0;
}

