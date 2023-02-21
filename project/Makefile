
CC := clang
LD := clang
ROOT := .

INCLUDE := ./inc
LIBS := -lkernel32 -lshell32 -lucrt -lgcc
ifeq ($(MODE),Debug)
LIBS += -lmingwex
endif

CFLAGS := -std=c99 -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -fuse-ld=lld
CFLAGS += -DUNICODE -D_UNICODE -D_WIN32 -DWIN32 -D_WIN64 -DWIN64
CFLAGS += -march=$(ARCH)

ifeq ($(MODE),Release)
CFLAGS += -Oz
CFLAGS += -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables
CFLAGS += -fno-math-errno
CFLAGS += -flto=thin
else ifeq ($(MODE),Debug)
CFLAGS += -Og -glldb -gfull -ggdb
CFLAGS += -fstack-protector-all
endif


LDFLAGS := -nostdlib -nostartfiles
LDFLAGS += -fuse-ld=lld
LDFLAGS += -Wl,--wrap=malloc -Wl,--wrap=free
LDFLAGS += -march=$(ARCH)

ifeq ($(MODE),Release)
LDFLAGS += -Oz
LDFLAGS += -s -Wl,--strip-all
LDFLAGS += -Wl,--gc-sections -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables
LDFLAGS += -flto=thin
else ifeq ($(MODE),Debug)
LDFLAGS += -Og -glldb -gfull -ggdb
LDFLAGS += -fstack-protector-all
endif


OBJ := ldd.o log.o main.o
OBJ := $(addprefix $(ROOT)/tmp/, $(OBJ))

all : $(OBJ)
	@echo -e "\e[0;32m ===== LINK \e[m"
	$(LD) $(LDFLAGS) $(LIBS) -Wl,-Map,$(ROOT)/tmp/lddw.map $^ -o $(ROOT)/bin/lddw.exe
	@echo -e "\e[0;32m ===== INFO \e[m"
	objdump -h $(ROOT)/bin/lddw.exe

$(ROOT)/tmp/%.o : $(ROOT)/src/%.c
	@echo -e "\e[0;32m ===== COMPILE : $< \e[m"
	@mkdir -p tmp
	@mkdir -p bin
	$(CC) $(CFLAGS) -I$(INCLUDE) -c -o $@ $<

$(OBJ) :

.PHONY: clean
clean:
	@echo -e "\e[0;32m ===== CLEAN \e[m"
	rm -rf tmp
	rm -rf bin
