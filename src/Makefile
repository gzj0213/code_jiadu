# KERNELDIR：开发板所使用的Linux内核源码目录
KERNELDIR := /home/kial/PLC/rk3588/kernel
# CURRENT_PATH：当前路径，通过运行“pwd”命令获取
CURRENT_PATH := $(shell pwd)
# obj-m：将 chrdevbase.c 这个文件编译为chrdevbase.ko模块
CROSS_COMPILE :=/home/kial/PLC/rk3588/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
#CROSS_COMPILE :=/home/kial/OPS/rk3399-debian/rk3399/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
ENV:=ARCH=arm64
CC:=$(CROSS_COMPILE)gcc
obj-m := gpio_test.o

build: kernel_modules
# -C 表示切换工作目录到KERNERLDIR目录
# M 表示模块源码目录
# modules 表示编译模块
kernel_modules:
	#$(MAKE)	-C $(KERNELDIR) M=$(CURRENT_PATH)	$(ENV)	$(CROSS_COMPILE) modules
	$(MAKE) -C $(KERNELDIR) M=`pwd` ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE)  modules
clean:
	$(MAKE) $(ENV)	-C $(KERNELDIR) M=$(CURRENT_PATH) clean
