//Assignment 1,  Vishal Chekale , 2021H1400157P

#define MAGIC_NUM 100   
#define IOCTL_GET_allignment _IOW(MAGIC_NUM, 0, char *) //select alignment of data in register
#define IOCTL_GET_Channel _IOW(MAGIC_NUM, 1, int *) //select channel of adc
#define SEL_CONV _IOR(MAGIC_NUM, 3, int *)
#define DEVICE_FILE_NAME "/dev/adc-dev"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>              // open
#include <unistd.h>             // exit 
#include <sys/ioctl.h>          // ioctl
#include<stdint.h>             // typedefs that specify exact width integer types

int ioctl_set_channel(int file_desc, int *ch)   // this ioctl call is used to set channel 1-8
{
    int ret_val;
    ret_val = ioctl(file_desc, IOCTL_GET_Channel, ch);

    if (ret_val < 0) 
    {
        printf("ioctl_sel_channel failed:%d\n", ret_val);
        exit(-1);
    }
       
    return 0;
}

int ioctl_set_alignment(int file_desc,char *al)  // this ioctl call is used to set the allignment right or left
{
    int ret_val;
    
    ret_val = ioctl(file_desc,IOCTL_GET_allignment, al);

    if (ret_val < 0) 
    {
        printf("ioctl_sel_channel failed:%d\n", ret_val);
        exit(-1);
    }

        
    return 0;
}


int ioctl_sel_conv(int file_desc, int conv)
{
    int ret_val;
    ret_val = ioctl(file_desc, SEL_CONV, conv );
    if (ret_val < 0)
    {
        printf("IOCTL allignment failed:%d\n", ret_val);
        exit(-1);
    }
    return 0;
}


int main()
{
    int file_desc, ret_val;
    int adc_ch_no;
    char align;
    uint16_t ADC_out;
    int conv;

    file_desc = open(DEVICE_FILE_NAME, 0);
    if (file_desc < 0) 
    {
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

        
    printf("Please select a ADC channel from 1 to 8 : ");
    scanf("%d",&adc_ch_no); //Select the channel

    printf("Channel selected is : %d\n",adc_ch_no);
	if(adc_ch_no<0||adc_ch_no>8)
     {
	printf("invalid channel selection\n");
        return 0;
     }

     ioctl_set_channel(file_desc,&adc_ch_no);


    printf("Please select the Data alignment 'L' for Left or 'R' for Right : ");
    scanf("%s",&align);                         //Select the alignment
       if(align=='L'||align=='R')
	{
    
        ioctl_set_alignment(file_desc,&align);

	}
	else 
        {
	printf("Invalid Data alignment selected\n");
	return 0;
	}
    	
	if(align=='L')
		{
	          ADC_out = ADC_out>>4;
		
		}
		
		
		printf("Enter type of covesrion( 0-1):");
        scanf("%d", &conv);
        
	read(file_desc,&ADC_out,2); 	
	
	printf("The ADC data from selected Channel no. %d is = %u\n",adc_ch_no,ADC_out);
	
	
	
	if (conv ==0)			
        {
         printf("one shot execution ");
        }

	else
	{
 	for(int i=0;i<50;i++)
 	{ 
  	read(file_desc,&ADC_out,2);   //Reading the values from device (adc-dev) file
    	printf("\n DATA read by the user is : %x --", ADC_out);
	}
	}

    		
       close(file_desc);             // Closing the file_desc file
       return 0;
}
