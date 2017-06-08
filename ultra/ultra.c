/*
 * TODO : 
 * 1) 초음파센서 ECHO PIN 회로 작성 및 구성
 *
 * 2) 초음파센서의 TRIG 신호 발생시 시간과 ECHO 신호 수신시 시간의 차이를 명확하게 구해야함
 *   
 * 3) 외부인터럽트 확인 EINT9,10,11,12,13,14,15,22
 */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include <linux/init.h>


#include <linux/cdev.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/ioctl.h>

#define DRIVER_AUTHOR 	"YOON"
#define DRIVER_DESC 	"ultra sonic driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_DESC);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");

#define TRIG	0
#define ECHO	1

#define TRIGPIN		1
#define ECHOPIN		2

struct timeval after, before;
u32 irq;
static unsigned int cms;
static int ultra_major = 0, ultra_minor = 0;

static int result;

static dev_t ultra_dev;
struct file_operations ultra_fops;
static struct cdev ultra_cdev;

static int ultra_sonic_register_cdev(void);

static int ultra_open(struct inode *inode,struct file *filp);
static int ultra_release(struct file *filp,const char *buf,size_t count, loff_t *f_pos);
static int ultra_read(struct file *filp,char *buf,size_t count,loff_t *f_pos);

//APIS
int register_int_ISR(void);
void ultra_gpio_init(void);
void output_sonic_burst(void);
irqreturn_t int_interrupt(int irq, void *dev_id, struct pt_regs *regs);

int ultra_sonic_init(void);
void ultra_sonic_exit(void);


irqreturn_t int_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	//TODO
	printk("testirq\n");
	do_gettimeofday(&after);
	printk("%lu %lu\n",after.tv_usec, before.tv_usec);
	cms = (((unsigned long)(after.tv_usec - before.tv_usec))/58);
	printk("%ld cm\n",cms);

	return IRQ_HANDLED;
}

static int ultra_open(struct inode *inode,struct file *filp)
{
	printk("ultra sonic has been opend ...\n");
	return 0;
}

static int ultra_release(struct file *filp,const char *buf,size_t count, loff_t *f_pos)
{
	printk("ultra sonic has been closed...\n");
	return 0;
}


static int ultra_read(struct file *filp,char *buf,size_t count,loff_t *f_pos)
{
	char data[11];
	output_sonic_burst();
	sprintf(data,"%ld",cms);
	copy_to_user(buf,data,count);
	printk("copl\n");
	return 0;
}

static int ultra_sonic_register_cdev()
{
	int error;
	if(ultra_major){
		ultra_dev=MKDEV(ultra_major,ultra_minor);
		error=register_chrdev_region(ultra_dev,1,"ultra");
	}else
	{
		error=alloc_chrdev_region(&ultra_dev,ultra_minor,1,"ultra");
		ultra_major=MAJOR(ultra_dev);
	}
	if(error<0){
		printk(KERN_WARNING "ultra:can'get major %d\n",ultra_major);
		return error;
	}
	printk("major number : %d\n",ultra_major);

	printk("minor number : %d\n",ultra_minor);

	cdev_init(&ultra_cdev,&ultra_fops);
	ultra_cdev.owner = THIS_MODULE;
	ultra_cdev.ops = &ultra_fops;
	error =cdev_add(&ultra_cdev,ultra_dev,1);

	if(error)
		printk(KERN_NOTICE "ultra Register Error %d",error);

	return 0;

}

int ultra_sonic_init(void)
{
	printk("Ultra sonic device driver on!\n");
	if((result=ultra_sonic_register_cdev())<0)
	{
		return result;
	}
	ultra_gpio_init();
	register_int_ISR();
	return 0;
}
void ultra_sonic_exit(void)
{


	free_irq(IRQ_EINT10,0x01);
	unregister_chrdev_region(ultra_dev,1);

	cdev_del(&ultra_cdev);

	printk("ultra sonic off\n");
}

int register_int_ISR(void)
{
	if(request_irq(IRQ_EINT10,(void*)int_interrupt,IRQF_TRIGGER_FALLING,"ultra",0x01))
	{
		return -EINVAL;
	}
	return 0;
}
void ultra_gpio_init(void)
{
	//TODO gpio_config set

	gpio_set_value(S3C2410_GPG(TRIGPIN),0);
	s3c_gpio_cfgpin(S3C2410_GPG(TRIGPIN),S3C_GPIO_SFN(1));  //OUTPUT
	s3c_gpio_cfgpin(S3C2410_GPG(ECHOPIN),S3C_GPIO_SFN(2));  //EINT10

		
}
void output_sonic_burst(void)
{
	//TODO Trig,timer on
	gpio_set_value(S3C2410_GPG(TRIGPIN),1);
	udelay(10);
	gpio_set_value(S3C2410_GPG(TRIGPIN),0);
	do_gettimeofday(&before);
}

struct file_operations ultra_fops={
	.open = ultra_open,
	.release = ultra_release,
	.read = ultra_read,
};



module_init(ultra_sonic_init);
module_exit(ultra_sonic_exit);

