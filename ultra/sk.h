/*
 * sk.h header file
 */

#ifndef _SK_H_
#define _SK_H_

typedef struct _led
{
	int num;
	int state;
}LED;

#define SK_MAGIC	'k'
#define SK_MAXNR	1 	

#define LED_OFF	1
#define LED_ON	0

#define SK_LED	_IOW(SK_MAGIC,0,LED)

#endif
