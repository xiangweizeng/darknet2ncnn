DEBUG=0

VPATH=./
CLASSIFIER=classifier
YOLO=yolo
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
LDFLAGS+= `pkg-config --libs opencv` -lstdc++
COMMON+= `pkg-config --cflags opencv` 

CLASSIFIEROBJA=classifier.o
YOLOOBJA=yolov2.o

CLASSIFIEROBJ = $(addprefix $(OBJDIR), $(CLASSIFIEROBJA))
YOLOOBJ = $(addprefix $(OBJDIR), $(YOLOOBJA))
DEPS = $(wildcard *.h) Makefile

all: obj  $(CLASSIFIER) $(YOLO)

$(CLASSIFIER): $(CLASSIFIEROBJ)
	$(CC)  $^ -o $@ $(LDFLAGS) $(COMMON) $(CFLAGS)

$(YOLO): $(YOLOOBJ)
	$(CC)  $^ -o $@ $(LDFLAGS) $(COMMON) $(CFLAGS)

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CPP) $(COMMON) $(CFLAGS) -c $< -o $@

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

%.cifar: $(CLASSIFIER) 
	./classifier zoo/$(basename $@).param  zoo/$(basename $@).bin  data/32516_dog.png data/cifar_lable.txt

%.imagenet: $(CLASSIFIER)
	./classifier zoo/$(basename $@).param  zoo/$(basename $@).bin  data/dog.jpg  data/imagenet.shortnames.list

%.coco: $(YOLO)
	./yolo zoo/$(basename $@).param  zoo/$(basename $@).bin  data/dog.jpg  data/coco.names

%.voc: $(YOLO)
	./yolo zoo/$(basename $@).param  zoo/$(basename $@).bin  data/dog.jpg  data/voc.names

cifar.cifar:

alexnet.imagenet: 
darknet.imagenet:
darknet19.imagenet:
darknet53.imagenet:
densenet201.imagenet:
extraction.imagenet:
resnet18.imagenet:
resnet34.imagenet:
resnet50.imagenet:
resnet101.imagenet:
resnet152.imagenet:
resnext50.imagenet:
resnext101-32x4d.imagenet:
resnext152-32x4d.imagenet:
vgg-16.imagenet:

yolov1-tiny.voc:
yolov2-tiny.coco:
yolov2.coco:
yolov3-tiny.coco:
yolov3.coco:
yolov3-spp.coco:

obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(OBJS) $(CLASSIFIER) $(YOLO) $(CLASSIFIEROBJ) $(OBJDIR)/*

