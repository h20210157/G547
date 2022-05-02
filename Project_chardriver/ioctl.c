
#include "chardev.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>              
#include <unistd.h>             
#include <sys/ioctl.h>          /* ioctl */
#include <string.h>
#include "chardev.h"

#define CONFIG_PARAM 4
typedef unsigned char		u8;
static int range = 0;   //variable that decides the range.

//user function for setting power control register
int ioctl_pwr_ctl(int file_desc)
{
    
	printf("power on :)\n");
    ioctl(file_desc, IOCTL_PWR_CTL);

    return 0;
}
//user function to read x axis value
int acc_x_read(int file_desc)
{
    int ret_val;
    int message;

    ioctl(file_desc, ACC_X_READ, &message);
	message=message&0x03ff;// data is in 10 bit (resolution) 
 	if(message>512)	// for signed number , if number is negative , take 2's complement
	message =message-1024; 

    	if(range == 0){	//if range set is 0
	message = message * 3.90; } //sensitivity is 3.9 mG/LSB

	else if(range == 1){	//if range is 1
	message = message * 7.8; }//sensitivity is 7.8 mG/LSB

	else if(range == 2){//If range is 2
	message = message * 15.6; }//sensitivity is 15.6 mG/LSB

	else if(range == 3){//if range is 3
	message = message * 31.2; }//sensitivity is 31.2 mG/LSB}

	  
	printf("acc_x_read:%d mG\n", message);

       return 0;
}

int acc_y_read(int file_desc)
{
    	int ret_val;
   	int message;

    	ioctl(file_desc, ACC_Y_READ, &message);
	message=message&0x03ff;		// data is in 10 bit (resolution) 
 	if(message>512)	// for signed number , if number is negative , take 2's complement
	message =message-1024; 
	
	if(range == 0){	//if range set is 0
	message = message * 3.90; } //sensitivity is 3.9 mG/LSB

	else if(range == 1){	//if range is 1
	message = message * 7.8; }//sensitivity is 7.8 mG/LSB

	else if(range == 2){//If range is 2
	message = message * 15.6; }//sensitivity is 15.6 mG/LSB}

	else if(range == 3){//if range is 3
	message = message * 31.2; }//sensitivity is 31.2 mG/LSB}

	printf("acc_y_read:%d mG\n", message);
    	return 0;
	
	
	
	
}
int acc_z_read(int file_desc)
{
    int ret_val;
    int message;

    ioctl(file_desc, ACC_Z_READ, &message);

 	message=message&0x03ff;		// data is in 10 bit (resolution) 
 	if(message>512)	// for signed number , if number is negative , take 2's complement
	message =message-1024; 
	
	if(range == 0){	//if range set is 0
	message = message * 3.90; } //sensitivity is 3.9 mG/LSB

	else if(range == 1){	//if range is 1
	message = message * 7.8; }//sensitivity is 7.8 mG/LSB

	else if(range == 2){//If range is 2
	message = message * 15.6; }//sensitivity is 15.6 mG/LSB}

	else if(range == 3){//if range is 3
	message = message * 31.2; }//sensitivity is 31.2 mG/LSB}

 
	printf("acc_x_read:%d mG\n", message);
    	return 0;
}

//range setting - function user space
int range_setting(int file_desc, u8 range_variable)
{
    int ret_val;
    

    ioctl(file_desc, RANGE_SETTING, &range_variable);

    printf("range setting \n");

    
    return 0;
}


int main()
{
    int file_desc; 
    int msg=64;
   

    file_desc = open(DEVICE_FILE_NAME, 0);
    if (file_desc < 0) {
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

   
//user space call to functions	
	ioctl_pwr_ctl(file_desc);//power controlling
	range_setting(file_desc, range);   // 0: 2g, 1: 4g, 2:8g, 3:16g
	//functions to read x,y,z axis values
	acc_x_read(file_desc);
	acc_y_read(file_desc);
	acc_z_read(file_desc);
	
    close(file_desc);
    return 0;
}
