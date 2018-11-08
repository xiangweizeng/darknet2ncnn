DEBUG=0

VPATH=./
BENCHMARK=benchdarknet
OBJDIR=./obj/

CC=gcc
CPP=g++
AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS=  -L .. -L ../ncnn/build/install/lib/  -lncnn -ldarknet2ncnn -lm -pthread -fopenmp 
COMMON=  -I ../include -I ../ncnn/src/ -I ../ncnn/build/install/include/  
CFLAGS= -Wno-unused-result  -Wfatal-errors -fPIC

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)
LDFLAGS+= -lstdc++

BENCHMARKOBJA=benchncnn.o

BENCHMARKOBJ = $(addprefix $(OBJDIR), $(BENCHMARKOBJA))
YOLOOBJ = $(addprefix $(OBJDIR), $(YOLOOBJA))
DEPS = $(wildcard *.h) Makefile

all: obj  $(BENCHMARK) $(YOLO)

$(BENCHMARK): $(BENCHMARKOBJ)
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

