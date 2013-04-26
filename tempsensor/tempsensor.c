/*
 * tempsensor.c
 *
 * Based on generic_hid.c by Jan Axelson (Apr 22, 2011)
 *
 * License of generic_hid.c:
 * 
 * Licensor grants any person obtaining a copy of this software ("You")
 * a worldwide, royalty-free, non-exclusive license, for the duration of
 * the copyright, free of charge, to store and execute the Software in a
 * computer system and to incorporate the Software or any portion of it
 * in computer programs You write.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * 
 * 
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

// Assumes interrupt endpoint 2 IN and OUT:
static const int INTERRUPT_IN_ENDPOINT = 0x81;
static const int INTERRUPT_OUT_ENDPOINT = 0x01;
	
// With firmware support, transfers can be > the endpoint's max packet size.
static const int MAX_INTERRUPT_IN_TRANSFER_SIZE = 5;
static const int MAX_INTERRUPT_OUT_TRANSFER_SIZE = 5;

/*
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
*/

void help() {

	printf("TempSensor 0.1 [options]\n");
	printf("Options:\n");
	printf("-v = Verbose printout\n");
	printf("-t = Print temperature value and exit\n");
	printf("-u = Print humidity value and exit\n");
	printf("-h = Help, this printout\n");
	exit(0);
	
}

void printout(char *str, int value) {
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	printf("%04d-%02d-%02d %02d:%02d:%02d, ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	if (value == 0) {
		printf("%s\n", str); 
	} else {
		printf("%s %d\n", str, value); 
	}
}

int main(int argc, char *argv[])
{

	// Command line arguments
	int verbose = 0;
	int temperature = 0;
	int humidity = 0;

	while ((argc > 1) && (argv[1][0] == '-'))
	{
		switch (argv[1][1])
		{
			case 'v':
				verbose = 1;
				break;
			
			case 't':
				temperature = 1;
				break;
			
			case 'u':
				humidity = 1;
				break;
			
			case 'h':
				help();
				
		}
		
		++argv;
		--argc;
	}

	if (verbose == 1) {
		printout("TempSensor 0.1", 0);
	}

	static const int VENDOR_ID = 0x1781;
	static const int PRODUCT_ID = 0x0ec4;
	
	struct libusb_device_handle *devh = NULL;
	int device_ready = 0;
	int result;

	if (verbose == 1) {
		printout("Init sensor", 0);
	}
	
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
					if (verbose == 1) {
						printout("USB Device ready", 0);
					}
				} else {
					fprintf(stderr, "Error: libusb_claim_interface error %d\n", result);
				}
			}
			
		} else {
			fprintf(stderr, "Error: Unable to find the device.\n");
		}
		
	} else {
		fprintf(stderr, "Error: Unable to initialize libusb.\n");
	}
	
	if (device_ready)
	{

		if (verbose == 1) {
			printout("Read device", 0);
		}
		
		// Send and receive data.
		// read_usb(devh);
		
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
				if (verbose == 1) {
					printout("Data received:", 0);
				}
			
				for(i = 0; i < bytes_transferred; i++) {
					printf("%02x ",data_in[i]);
				}
				printf("\n");
				
				# Check if temp or hum
				if (data_in[0] == 2)
				{
					printout("Humidity", 0);
				}
				if (data_in[0] == 3)
				{
					printout("Temperature", 0);
				}
				
			} else {
				fprintf(stderr, "Error: No data received (%d)\n", result);
			}
		} else {
			fprintf(stderr, "Error receiving data via interrupt transfer %d\n", result);
		}
		
		// Finished using the device.
		if (verbose == 1) {
			printout("Release interface", 0);
		}
		libusb_release_interface(devh, 0);
	}
	
	if (verbose == 1) {
		printout("Close USB Device", 0);
	}

	libusb_close(devh);
	libusb_exit(NULL);
	return 0;
}

