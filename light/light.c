//TODO : 밝기 조절 함수 통합


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <asm/delay.h>
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <plat/devs.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/gpio.h>
#include <linux/ioctl.h>

#include <linux/time.h>
#include <linux/timer.h>

#include "light.h"
//#include <linux/platform_data/leds-s3c24xx.h>
#define DRIVER_AUTHOR 	"YOON"
#define DRIVER_DESC 	"sample driver"



#define ARGS	20000
#define STEP_D	1
#define MAX_LEVEL 	(((((STEP_D))*100)-1))
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_DESC);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");
struct file_operations light_fops;

struct pwm_device{
	struct list_head	list;
	struct platform_device	*pdev;

	struct clk		*clk_div;
	struct clk		*clk;
	const char		*label;

	unsigned int		period_ns;
	unsigned int		duty_ns;

	unsigned char		tcon_base;
	unsigned char		running;
	unsigned char		use_count;
	unsigned char		pwm_id;
};

struct pwm_config_data {
	int duty_ns;
	int period_ns;
};

static struct pwm_device *light_pwm;
static struct pwm_config_data light_config;
static int light_open(struct inode *inode,struct file *filp);
static int light_release(struct inode *inode,struct file *filp);
static long light_ioctl(struct file *filp,unsigned int cmd,unsigned long arg);
static unsigned int light_level=0;

static void light_brightness(unsigned int level);
static void light_brightness_setting(int flag,unsigned int set);
static void light_off(void);

int light_init(void);
void light_exit(void);

struct file_operations light_fops = {
	.open		= light_open,
	.release	= light_release,
	.unlocked_ioctl = light_ioctl,
};


static void light_brightness_setting(int flag,unsigned int set)
{
	int level=0;
	if(light_level<0) light_level =0;
	if(flag!=1&&set>30) set = 30;
	if(light_level<set){	
		for(level=light_level;level<set;level++)
		{
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			light_brightness(level);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			printk("level %d\n",level);
			light_level=level;
		}
	}
	else if(light_level>set)
	{	
		for(level=light_level;level>set;level--)
		{
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			light_brightness(level);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			udelay(800);
			printk("level %d\n",level);
			light_level=level;
		}
	}else if(light_level==set)
	{
		light_brightness(level);
	}else{
		light_brightness(level);
	}

}
static void light_off(void)
{
	pwm_disable(light_pwm);
	s3c_gpio_cfgpin(S3C2410_GPB(3),S3C_GPIO_SFN(1));
	gpio_set_value(S3C2410_GPB(3),1);
	udelay(10);
	s3c_gpio_cfgpin(S3C2410_GPB(3),S3C_GPIO_SFN(2));
	light_level=0;
	printk("level %d\n",light_level);
}


static void light_brightness(unsigned int level)
{	
	pwm_disable(light_pwm);
	if(level*STEP_D>100) level=100/(STEP_D+1);	
	if(level==0){
			light_config.duty_ns=ARGS-10;
			light_config.period_ns=ARGS;
		}else if(level==(100/(STEP_D+1))){
			light_config.duty_ns=10;
			light_config.period_ns=ARGS;
		}else{
			light_config.duty_ns=ARGS-(STEP_D*200*level);
			light_config.period_ns=ARGS;
		}
	pwm_config(light_pwm, light_config.duty_ns, light_config.period_ns);
	pwm_enable(light_pwm);
}
int light_open(struct inode *inode,struct file *filp)
{
	int cds;
	light_level=0;
	cds=!(gpio_get_value(S3C2410_GPG(3)));

	light_off();
	return 0;
}

static int light_release(struct inode *inode,struct file *filp)
{
	pwm_disable(light_pwm);
	pwm_free(light_pwm);
	return 0;
}

static long light_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	int cds;
	static unsigned int level;
	cds=gpio_get_value(S3C2410_GPG(3));
	if(_IOC_TYPE(cmd)!=DEV_LIGHT_IOCTL_MAGIC) return -EINVAL;
	if(_IOC_NR(cmd) >= DEV_LIGHT_IOCTL_MAXNR) return -EINVAL;
	copy_from_user((void *)&level,(const void*)arg,sizeof(unsigned int));

	if(level==0) light_off();
	if(level!=0) light_brightness_setting(cds,level);
	
	
}


int light_init(void)
{	
	register_chrdev(251,"light",&light_fops);
	s3c_gpio_cfgpin(S3C2410_GPB(3),S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S3C2410_GPB(3),S3C_GPIO_PULL_UP);
	s3c_gpio_cfgpin(S3C2410_GPG(3),S3C_GPIO_SFN(2));
	//int ARGS=1000;
	light_pwm=pwm_request(3,"light");
	if(light_pwm==NULL)
	{
		printk("pwm_request fail\n");
		return -1;
	}
	//pwm_disable(light_pwm);
	printk("Insert light module!\n");
	return 0;

}
void light_exit(void)
{

	pwm_free(light_pwm);
	pwm_disable(light_pwm);
	s3c_gpio_cfgpin(S3C2410_GPB(3),S3C_GPIO_SFN(1));
	gpio_set_value(S3C2410_GPB(3),1);
	printk("Remove light module!\n");
	unregister_chrdev(251,"light");
}


module_init(light_init);
module_exit(light_exit);

