ODIR = ../../build

ASM = /v1/bin/nasm
FLAGS = -felf
FLAGS := $(FLAGS) -dD_ENABLE_GRAPHICS=$(ENABLE_GRAPHICS)

_OBJS = $(_OBJS_ASM)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
SRC = $(_OBJS:%.o=%.asm)

all: $(OBJS)

../../build/%.o: %.asm
	$(ASM) -o $(ODIR)/$(subst .asm,.o,$^) $^ $(FLAGS)

clean: