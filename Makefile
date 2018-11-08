DEBUG=0

VPATH=./src/:./src/layer:./
CONVERT=darknet2ncnn
VERIFY=convert_verify
EXT_LIB=libdarknet2ncnn.a
OBJDIR=./obj/

CC=gcc
CPP=g++
AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS=  -L ncnn/build/install/lib/ -L .  -L darknet  -ldarknet -lncnn -ldarknet2ncnn -lm  -pthread -fopenmp
COMMON= -I include  -I ncnn/build/install/include/ -I darknet/include/ -I src -I ncnn/src/
CFLAGS= -Wno-unused-result  -Wfatal-errors -fPIC

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)
LDFLAGS+= `pkg-config --libs opencv` -lstdc++
COMMON+= `pkg-config --cflags opencv` 

CONVERT_OBJA=darknet2ncnn.o
VERIFY_OBJA=convert_verify.o
EXT_LIB_OBJA= darknet_activation.o \
darknet_shortcut.o \
yolov1_detection.o \
yolov3_detection.o \
object_detection.o \
register_darknet.o 

CONVERT_OBJ = $(addprefix $(OBJDIR), $(CONVERT_OBJA))
VERIFY_OBJ  = $(addprefix $(OBJDIR), $(VERIFY_OBJA))
EXT_LIB_OBJ = $(addprefix $(OBJDIR), $(EXT_LIB_OBJA))
DEPS = $(wildcard *.h) Makefile

all: obj  $(CONVERT) $(VERIFY) $(EXT_LIB)

$(VERIFY): $(VERIFY_OBJ)  $(EXT_LIB)
	$(CC)  $^ -o $@ $(LDFLAGS) $(COMMON) $(CFLAGS)

$(CONVERT): $(CONVERT_OBJ)  $(EXT_LIB)
	$(CC)  $^ -o $@ $(LDFLAGS) $(COMMON) $(CFLAGS)

$(EXT_LIB): $(EXT_LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CPP) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

%.net:  $(VERIFY) $(CONVERT)
	./darknet2ncnn data/$(basename  $@).cfg  data/$(basename  $@).weights example/zoo/$(basename  $@).param  example/zoo/$(basename  $@).bin 
	./convert_verify data/$(basename  $@).cfg  data/$(basename  $@).weights example/zoo/$(basename  $@).param  example/zoo/$(basename  $@).bin example/data/dog.jpg

obj:
	mkdir -p obj


cifar: $(VERIFY) $(CONVERT)
	./darknet2ncnn data/$@.cfg  data/$@.backup example/zoo/$@.param  example/zoo/$@.bin 
	./convert_verify data/$@.cfg  data/$@.backup example/zoo/$@.param  example/zoo/$@.bin  example/data/21263_ship.png

alexnet.net: 
darknet.net:
darknet19.net:
darknet53.net:
densenet201.net:
extraction.net:
resnet18.net:
resnet34.net:
resnet50.net:
resnet101.net:
resnet152.net:
resnext50.net:
resnext101-32x4d.net:
resnext152-32x4d.net:
vgg-16.net:
yolov1-tiny.net:
yolov2-tiny.net:
yolov2.net:
yolov3-tiny.net:
yolov3.net:
yolov3-spp.net:

all-net:cifar\
alexnet.net\
darknet.net\
darknet19.net\
darknet53.net\
densenet201.net\
extraction.net\
resnet18.net\
resnet34.net\
resnet50.net\
resnet101.net\
resnet152.net\
resnext50.net\
resnext101-32x4d.net\
resnext152-32x4d.net\
vgg-16.net\
yolov1-tiny.net\
yolov2-tiny.net\
yolov2.net\
yolov3-tiny.net\
yolov3.net\
yolov3-spp.net\

.PHONY: clean

clean:
	rm -rf $(OBJS) $(CONVERT) $(CONVERT_OBJ) $(EXT_LIB_OBJ)  $(EXT_LIB) $(VERIFY) $(VERIFY_OBJ) $(OBJDIR)/*

