.PHONY: clean
.DEFAULT_GOAL = ut

TOOLCHAIN = 

CFLAGS = -Werror -Wall

LIBS += -lbluetooth

OBJECTS += main.o
OBJECTS += find.o
OBJECTS += listen.o
OBJECTS += connect.o

%.o: %.c
	+@echo "building '$(notdir $<)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) -c -o $@ $< $(LIBS)

ut: $(OBJECTS)
	+@echo "linking '$(notdir $@)'"
	@$(TOOLCHAIN)gcc $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	@find . -name '*.o' -delete
	@rm -f $(OUTPUTS)
