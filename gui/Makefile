CC = i686-elf-g++
BUILD_TIMESTAMP := $(shell TZ='America/Chicago' date +"%D %r")
FLAGS = -ffreestanding -fno-rtti -fno-exceptions -Wno-int-to-pointer-cast -fpermissive -isystem../system-include -I../include -c
ODIR = ../../build

_OBJS = $(_OBJS_GUI)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
SRC = $(_OBJS:%.o=%.cpp)

FLAGS := $(FLAGS) -DD_ENABLE_GRAPHICS=$(ENABLE_GRAPHICS)

all: $(OBJS)


../../build/%.o: %.cpp
	$(CC) -o $(ODIR)/$(subst .cpp,.o,$^) $^ $(FLAGS)

clean:
	#nothing
