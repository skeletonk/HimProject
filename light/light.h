#ifndef __LIGHT_H__
#define __LIGHT_H__


#define DEV_LIGHT_IOCTL_MAGIC	'p'
unsigned int levels;

#define DEV_LIGHT_LEVEL		_IOW(DEV_LIGHT_IOCTL_MAGIC, 0, levels)
#define DEV_LIGHT_OFF		_IO(DEV_LIGHT_IOCTL_MAGIC,1)
#define DEV_LIGHT_IOCTL_MAXNR	2


#endif //__LIGHT_H_
