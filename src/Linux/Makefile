.PHONY: clean all
.DEFAULT_GOAL = all

TOOLCHAIN = 

CFLAGS = -Werror -Wall

LIBS += -lbluetooth -lpthread

%.o: %.c
	+@echo "building '$(notdir $<)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) -c -o $@ $< $(LIBS)

ut_RFCOMM: unitooth_RFCOMM.o
	+@echo "linking '$(notdir $@)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) $^ -o $@ $(LIBS)

ut_L2CAP: unitooth_L2CAP.o
	+@echo "linking '$(notdir $@)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) $^ -o $@ $(LIBS)

all: ut_RFCOMM ut_L2CAP

clean:
	@find . -name '*.o' -delete
	@rm -f $(OUTPUTS) ut
