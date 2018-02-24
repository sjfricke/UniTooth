.PHONY: clean all
.DEFAULT_GOAL = all

CFLAGS = 
TOOLCHAIN = 

LIBS += -lbluetooth

OUTPUTS += master.elf
OUTPUTS += slave.elf

%.o: %.c
	+@echo "building '$(notdir $<)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) -c -o $@ $< $(LIBS)

%.elf: %.o
	+@echo "linking '$(notdir $@)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) $^ -o $@ $(LIBS)

all: $(OUTPUTS)

clean:
	@find . -name '*.o' -delete
	@rm -f $(OUTPUTS)
