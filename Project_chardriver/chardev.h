/*
 *  chardev.h - the header file with the ioctl definitions.
 *
 *  The declarations here have to be in a header file, because
 *  they need to be known both to the kernel module
 *  (in main_1.c) and the process calling ioctl (ioctl.c)
 */

#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>

/*
 * The major device number. We can't rely on dynamic
 * registration any more, because ioctls need to know
 * it.
 */
#define MAJOR_NUM 100

#define IOCTL_PWR_CTL _IOR(MAJOR_NUM, 0,int *)

#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, int *)

#define RANGE_SETTING _IOWR(MAJOR_NUM, 2, int *)

#define ACC_X_READ _IOR(MAJOR_NUM, 3, int *)
#define ACC_Y_READ _IOR(MAJOR_NUM, 4, int *)
#define ACC_Z_READ _IOR(MAJOR_NUM, 5, int *)




#define DEVICE_FILE_NAME "/dev/adxl_file"


#endif
