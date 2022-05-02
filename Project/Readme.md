Writing IIO driver interfacing of ADXL345 sensor:

Prerequisite for the project

For these files to run we need raspberry pi running on linux kernel. To update and build the kernel use link. First remove the loaded module. Command to remove the module is "sudo rmmod adxl.ko"

List of softwares used for this project's implementation:

Putty
WinSCP
Vnc viewer


Summary

The following project implements a driver communicating with the ADXL345 over the I2C protocol, reads its data register, and returns its raw values to the user through a number of IOCTL calls implemented. Driver also facilitates the configuration of the range of the sensor reading using RANGE_SENSOR ioctl call. The host used is the Raspberry Pi 3b+ model.


Setup and the schematic.
Schematic: pi-and-adxl345_bb

Setup:

ADXL345

adxl

The ADXL345 is a MEMS producing output proportional to the acceleration that it is subjected to. It is able to measure so along 3 axes (x, y, z). It has 6 8-bit data registers, 2 for each axis, in effect 16 bits allocated to each axis. But not all the bits are used for the data interpretation. Depending upon the configuration of the DATA_FORMAT_REGISTER the number of bits actually associated with the data change. The Enable register also needs to be configured to get the sensor up and running. For further reference, refer the ADXL345 datasheet here.

Connections
ADXL Pin	RaspberryPi Pin
VCC	Pin 1 (3.3 V)
Ground	Pin 39
SCL	Pin 3
SDA	Pin 5
Driver functionality
The driver implemented has ioctl call for

Turning on the device
Reading individual axis data.
Setting the range to be +/-2g, +/-4g, +/-8g and +/- 16g.

Interpreting the data
The data given out by the sensor is in 2's compliment form. If the range is +/- 2g and resolution is 10 bit, then LSB will represent 4g/1024 (mulitpying factor) as an absolute value. Bringing the 2's compliment data back into decimal form and multiplying it by the factor will give us the value in Gs. The multiplying factor will change depending upon the range and the resolution.

Steps
Navigate to the directory conatining the driver and associated Makefile.
Execute the make command.
Insert the module -> sudo insmod adxl.ko
Grant the permission for the dev file
Execute the user file -> ./user_file
