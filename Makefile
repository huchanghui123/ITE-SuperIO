TARGET		?= $(shell uname -r)
KERNEL_MODULES	?= /lib/modules/$(TARGET)
KERNEL_BUILD	?= $(KERNEL_MODULES)/build
SYSTEM_MAP	?= $(KERNEL_BUILD)/System.map

DRIVER := my_gpio

obj-m	:= $(patsubst %,%.o,$(DRIVER))
obj-ko  := $(patsubst %,%.ko,$(DRIVER))

MAKEFLAGS += --no-print-directory

.PHONY: all modules clean

all: modules

# Targets for running make directly in the external module directory:

modules clean:
	@$(MAKE) -C $(KERNEL_BUILD) M=$(CURDIR) $@

