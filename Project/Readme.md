Writing IIO driver interfacing of ADXL345 sensor:

Prerequisite for the project
For these files to run we need raspberry pi running on linux kernel. To update and build the kernel use link. First remove the loaded module. Command to remove the module is "sudo rmmod adxl.ko"

List of softwares used for this project's implementation:
Putty
WinSCP
Vnc viewer

Summary
The following project implements a driver communicating with the ADXL345 over the I2C protocol, reads its data register, and returns its raw values to the user through a number of IOCTL calls implemented. Driver also facilitates the configuration of the range of the sensor reading using RANGE_SENSOR ioctl call. The host used is the Raspberry Pi 3b+ model.

