#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <asm/io.h>
#include <mach/gpio-fns.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include <linux/time.h>
#include <linux/timer.h>
#define DRIVER_AUTHOR 	"YOON"
#define DRIVER_DESC 	"sample driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_DESC);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");


//exturn value setting
#define TIME_STEP	(2*HZ/10)

typedef struct
{
	struct timer_list timer;
	unsigned long led;
}__attribute__((packed)) KERNEL_TIMER_MANAGER;

static KERNEL_TIMER_MANAGER *ptrmng =NULL;

void kerneltimer_timeover(unsigned long arg);
void kerneltimer_registertimer(KERNEL_TIMER_MANAGER *pdata,unsigned long timeover);
int kerneltimer_init(void);
void kerneltimer_exit(void);


void kerneltimer_timeover(unsigned long arg)
{
	KERNEL_TIMER_MANAGER *pdata = NULL;
	if(arg)
	{
		pdata =(KERNEL_TIMER_MANAGER *) arg;

		s3c2410_gpio_setpin(S3C2410_GPG(4),(pdata->led&0xFF));
		s3c2410_gpio_setpin(S3C2410_GPG(5),(pdata->led&0xFF));
		s3c2410_gpio_setpin(S3C2410_GPG(6),(pdata->led&0xFF));
		
		pdata->led =~(pdata->led);
		kerneltimer_registertimer(pdata,TIME_STEP);
	}
}
void kerneltimer_registertimer(KERNEL_TIMER_MANAGER *pdata,unsigned long timeover)
{
	init_timer(&(pdata->timer));
	pdata->timer.expires = get_jiffies_64() +timeover;
	pdata->timer.data = (unsigned long) pdata;
	pdata->timer.function = kerneltimer_timeover;

	add_timer(&(pdata->timer));
}
int kerneltimer_init(void)
{
	ptrmng = kmalloc(sizeof(KERNEL_TIMER_MANAGER),GFP_KERNEL);
	if(ptrmng==NULL) return -ENOMEM;

	memset(ptrmng,0,sizeof(KERNEL_TIMER_MANAGER));

	ptrmng->led=0;
	kerneltimer_registertimer(ptrmng,TIME_STEP);


		s3c2410_gpio_setpin(S3C2410_GPG(4),1);
		s3c2410_gpio_setpin(S3C2410_GPG(5),1);
		s3c2410_gpio_setpin(S3C2410_GPG(6),1);

		s3c2410_gpio_cfgpin(S3C2410_GPG(4),S3C2410_GPIO_OUTPUT);
		s3c2410_gpio_cfgpin(S3C2410_GPG(5),S3C2410_GPIO_OUTPUT);
		s3c2410_gpio_cfgpin(S3C2410_GPG(6),S3C2410_GPIO_OUTPUT);

	return 0;

}
void kerneltimer_exit(void)
{
	if(ptrmng!=NULL)
	{
		del_timer(&(ptrmng->timer));
		kfree(ptrmng);
	}
	
		s3c2410_gpio_setpin(S3C2410_GPG(4),1);
		s3c2410_gpio_setpin(S3C2410_GPG(5),1);
		s3c2410_gpio_setpin(S3C2410_GPG(6),1);
}


module_init(kerneltimer_init);
module_exit(kerneltimer_exit);

