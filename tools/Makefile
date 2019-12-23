DEBUG=0

VPATH=./
NCNN2MEM=ncnn2mem
NCNNOPTIMEIZE=ncnnoptimize
OBJDIR=./obj/

CC=gcc
CPP=g++
AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS=  -L .. -L ../ncnn/build/install/lib/ -ldarknet2ncnn  -lncnn -lm -pthread -fopenmp 
COMMON=  -I ../include -I ../src -I ../ncnn/src/ -I ../ncnn/build/install/include/ncnn
CFLAGS= -Wno-unused-result  -Wfatal-errors -fPIC

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)
LDFLAGS+= -lstdc++

NCNN2MEMOBJA=ncnn2mem.o
NCNNOPTIMEIZEOBJA=ncnnoptimize.o

NCNN2MEMOBJ = $(addprefix $(OBJDIR), $(NCNN2MEMOBJA))
NCNNOPTIMEIZEOBJ = $(addprefix $(OBJDIR), $(NCNNOPTIMEIZEOBJA))
DEPS = $(wildcard *.h) Makefile

all: obj  $(NCNN2MEM) $(NCNNOPTIMEIZE)

$(NCNN2MEM): $(NCNN2MEMOBJ)
	$(CC)  $^ -o $@ $(LDFLAGS) $(COMMON) $(CFLAGS)

$(NCNNOPTIMEIZE): $(NCNNOPTIMEIZEOBJ)
	$(CC)  $^ -o $@ $(LDFLAGS) $(COMMON) $(CFLAGS)

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CPP) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(OBJS) $(BENCHMARK) $(YOLO) $(BENCHMARKOBJ) $(OBJDIR)/*

