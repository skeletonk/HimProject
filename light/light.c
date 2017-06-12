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

//#include <linux/platform_data/leds-s3c24xx.h>
#define DRIVER_AUTHOR 	"YOON"
#define DRIVER_DESC 	"sample driver"

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

int light_init(void);
void light_exit(void);

struct file_operations light_fops = {
	.open		= light_open,
	.release	= light_release,
	.unlocked_ioctl = light_ioctl,
};

int light_open(struct inode *inode,struct file *filp)
{
	int args=1000;
	pwm_disable(light_pwm);
	light_config.duty_ns=args/2;
	light_config.period_ns=args;

	pwm_config(light_pwm, light_config.duty_ns, light_config.period_ns);
	pwm_enable(light_pwm);
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
	copy_from_user((void*)&light_config,(const void*)arg,sizeof(light_config));
	pwm_config(light_pwm, light_config.duty_ns, light_config.period_ns);
	pwm_enable(light_pwm);
}


int light_init(void)
{
	
	register_chrdev(251,"light",&light_fops);
	s3c_gpio_cfgpin(S3C2410_GPB(3),S3C_GPIO_SFN(2));
	s3c_gpio_setpull(S3C2410_GPB(3),S3C_GPIO_PULL_UP);

	//int args=1000;
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

