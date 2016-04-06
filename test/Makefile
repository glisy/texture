OS ?= $(shell uname)
SRC += $(wildcard ../deps/*/*.c)
SRC += $(wildcard ../*.a)
SRC += $(wildcard *.c)
CFLAGS += -I../include
CFLAGS += -I../deps
CFLAGS += -l glfw3

ifeq (Darwin, $(OS))
CFLAGS += -framework Foundation
CFLAGS += -framework OpenGL
endif

test: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@
	./$@

clean:
	rm -rf test
