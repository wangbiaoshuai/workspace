ARCH = $(shell getconf LONG_BIT)

all:

ifeq ($(ARCH), 64)
	@echo "yes"
else
	@echo "no"
endif
