CPU := power10

CC  := gcc
LINKER := $(CC)

# set lib/include paths
BLIS_PATH := /home/nicholai/prog/repos/blis
BLIS_INC  := $(BLIS_PATH)/include/$(CPU)
BLIS_LIB  := $(BLIS_PATH)/lib/$(CPU)/libblis.a


# set flags
ARCH_FLAGS := -mcpu=$(CPU) -mtune=$(CPU)
OPT_FLAGS  := -O3
ERR_FLAGS  := 
INC_FLAGS  := -I $(BLIS_INC)
LDFLAGS    := -lpthread -lm
CFLAGS     := $(ARCH_FLAGS) $(OPT_FLAGS) $(ERR_FLAGS) $(INC_FLAGS) $(MACROS)


# set compile objects
PERF_OBJS := performance.o
OBJS := $(PERF_OBJS)

all: block_test


# compile rule
$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(MACROS) -c $< -o $@


# linker
block_test: $(PERF_OBJS)
	$(LINKER) $(PERF_OBJS) $(BLIS_LIB) -o ./test_blocks.x $(LDFLAGS) 


# remove rule
clean:
	rm -rf *.x *.o

