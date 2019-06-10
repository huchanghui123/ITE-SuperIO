#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/miscdevice.h>
#include<linux/fs.h>
#include<linux/ioctl.h>

#include <linux/io.h>


/* IO Ports */
#define REG		0x2e    //Test 1 Register(地址端口index=2Eh)
#define VAL		0x2f    //Test 2 Register(数据端口index=2Fh)

/* Logical device Numbers LDN */ //LDN逻辑设备号入口O7H
#define GPIO		0x07
#define LDNREG		0x07

#define CHIPID 0x20

#define GPIO_BASE	0xa07
#define GPIO_BA_REG 0x62

#define IOC_MAGIC 'c'
#define IOC_COMMAND1 _IOW(IOC_MAGIC,0,int)
#define IOC_COMMAND2 _IOW(IOC_MAGIC,1,int)

#define DEVICE_NAME "my_gpio"

static inline int superio_enter(void)
{
	if (!request_muxed_region(REG, 2, DEVICE_NAME))
		return -EBUSY;
	outb(0x87, REG);
	outb(0x01, REG);
	outb(0x55, REG);
	outb(0x55, REG);
	return 0;
}

static inline void superio_exit(void)
{
	outb(0x02, REG);
	outb(0x02, VAL);
	release_region(REG, 2);
}

static inline void superio_select(int ldn)
{
	outb(LDNREG, REG);
	outb(ldn, VAL);
}

static inline int superio_inb(int reg)
{
	outb(reg, REG);
	return inb(VAL);
}

static inline void superio_outb(int reg, int val)
{
	outb(reg, REG);
	outb(val, VAL);
}

static inline int superio_inw(int reg)
{
	int val;
	outb(reg++, REG);
	val = inb(VAL) << 8;
	outb(reg, REG);
	val |= inb(VAL);
	return val;
}

int my_gpio_dev_open(struct inode *inode, struct file *filp)
{
	printk("my gpio dev open!\n");
	return 0 ;
}

int my_gpio_dev_close(struct inode *inode, struct file *filp)
{
	printk("my gpio dev close!\n");
	return 0 ;
}

void gpio_set_high_and_low(int val)
{
	u16 addr = 0xa07;
	outb(val,addr);
	pr_info("gpio set pin val:%02x\n",val);
}


long my_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
    printk("ioctl cmd:%d \n",cmd);
    switch(cmd)
    {
        case IOC_COMMAND1:
            printk("my gpio set high !\n");
            gpio_set_high_and_low(0xff);
            break;
        case IOC_COMMAND2:
            printk("my goio set low !\n");
            gpio_set_high_and_low(0x00);
            break;
        default:
            printk("my gpio ioctl error!\n");
            return -EFAULT;
    }
    return 0;
}


//初始化文件操作结构体
struct file_operations file_ops = {
	.owner	 = THIS_MODULE,
	.open	 = my_gpio_dev_open,
	.unlocked_ioctl = my_ioctl,
	.release = my_gpio_dev_close,
};

//初始化misc设备结构体
struct miscdevice my_gpio_dev = {
	//由内核自动分配次设备号
	.minor = MISC_DYNAMIC_MINOR ,
	//初始化设备名称
	.name = DEVICE_NAME ,
	//初始化文件操作结构体
	.fops = &file_ops,	
};


static int __init my_gpio_dev_init(void) 
{
	int ret_error ;
	int rc = 0;
	u16 chip_type, gpio_io_base;

	//注册misc设备
	int ret = misc_register(&my_gpio_dev);
	if(ret != 0){
	   ret_error = ret ;
	   printk("misc register fair!\n");
	   goto fair ;
	}
	printk("misc init success!\n");	

	rc = superio_enter();
	if (rc)
		goto exit;

	superio_select(GPIO);
	superio_outb(0x2c, 0x89);
	superio_outb(0xcf, 0xff);

	chip_type = superio_inw(CHIPID);
	gpio_io_base = superio_inw(GPIO_BA_REG);
	pr_info("Found Chip IT%04x GPIO lines starting at %04xh\n", chip_type, gpio_io_base);

	outb(0x00,gpio_io_base+7);
    
	superio_exit();
	return ret ;

fair:
	return ret_error ;
exit:
	return rc;

}
 
static void __exit my_gpio_dev_exit(void) 
{
	//注销misc设备
	misc_deregister(&my_gpio_dev);
	printk("my gpio unregister!\n");
}

module_init(my_gpio_dev_init);
module_exit(my_gpio_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("LAOHU add gpio driver");
