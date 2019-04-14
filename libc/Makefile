CC = i686-elf-gcc
BUILD_TIMESTAMP := $(shell TZ='America/Chicago' date +"%D %r")
FLAGS = -std=gnu99 -fno-builtin -ffreestanding -Wno-int-to-pointer-cast -isystem../system-include -I../include -c
ODIR = ../../build

_OBJS = $(_OBJS_LIB_C)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
SRC = $(_OBJS:%.o=%.c)

FLAGS := $(FLAGS) -DD_ENABLE_GRAPHICS=$(ENABLE_GRAPHICS)

all: $(OBJS)


../../build/%.o: %.c
	$(CC) -o $(ODIR)/$(subst .c,.o,$^) $^ $(FLAGS)

clean:
	#nothing for now
