#include<stdio.h>
#include<sys/io.h>
#include<stdlib.h>

/* IO Ports */
#define REG		0x2e
#define VAL		0x2f
/* Logical device Numbers LDN */
#define LDNREG		0x07
#define GPIO		0x07

static int superio_inw(int reg)
{
	int val;
	outb(reg++, REG);
	val = inb(VAL) << 8;
	outb(reg, REG);
	val |= inb(VAL);
	return val;
}

int main(void)
{   
	unsigned short int io_base;

	//修改端口权限为可读写
	int ret = iopl(3);
	if(ret == -1)
	{
		printf("iopl error.\n");
		return -1;
	}
	//进入IO 配置空间
	outb(0x87, REG);
	outb(0x01, REG);
	outb(0x55, REG);
	outb(0x55, REG);
	
	outb(LDNREG, REG);
	outb(GPIO, VAL);
	//Speecial Function Selection Register3(Index=2Ch, Default=89h)
	outb(0x2c, REG);
	outb(0x89, VAL);

	//Simple I/O Base Address MSB Register index=62h
	//获取GPIO基地址,copied from gpio-it87.c
	io_base = superio_inw(0x62);
	printf("gpio base addr:%02x\n", io_base);

	//Output/Input Selection (Index CFh）
	//0xcf 控制GP80~87 作为输入还是输出,相应的bit位设为0为输入，设为1为输出
	outb(0xcf, REG);
	//bit0对应GP80,bit1对应GP81,以此类推。以下是把所有IO作为输出
	outb(0xff, VAL);

	//当IO作为输出时，拉高、或拉低设置
	//使用的第8组GPIO，这里地址是基地址+7
	//控制GP80~87输出高还是低,相应的BIT 设为0拉低，设为1拉高
	//bit0对应GP80,bit1对应GP81,以此类推。这里所有IO设置为高电平
	outb(0x00,io_base+7);
	//设置完后退出IO配置空间
	outb(0x02, REG);
	outb(0x02, VAL);
	//恢复端口权限
	iopl(0);

	return 0;
}