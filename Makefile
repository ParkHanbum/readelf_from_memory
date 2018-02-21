VERSION := 0.1.0


CFLAGS := -D_GNU_SOURCE
LDFLAGS := -ldl


ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g -gdwarf -ggdb
else 
	CFLAGS += -O2
endif


SRCS := $(wildcard *.c)

refm: $(SRCS)
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)


clean:
	rm refm $(wildcard *.o)
