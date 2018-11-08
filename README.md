# darknet2ncnn

# Introduction
Darknet2ncnn converts the darknet model to the ncnn model, enabling rapid deployment of the darknet network model on the mobile device.
1. Support network layers except local/xor conv, rnn, lstm, gru, crnn and iseg
1. Added all activation operations not directly supported by ncnn, implemented in the layer DarknetActivation
1. Added the implementation of the shortcut layer, implemented in the layer DarknetShortCut
1. Added yolo layer and detection layer implementation, support YOLOV1 and YOLOV3
1. Provides a converted model verification tool, convert_verify, which supports checking the calculation output of each layer of the network, supports convolutional layer parameter checking, and facilitates rapid positioning of problems in model conversion.

# Install&Usage

1. Install opencv-dev, gcc, g++, make, cmake

2. Download source
```sh
git clone https://github.com/xiangweizeng/darknet2ncnn.git
```
3. Init submodule
```sh
cd darknet2ncnn
git submodule init
git submodule update
```
4. build darknet
```sh
cd darknet2
make -j8
rm libdarknet.so
```
5. build ncnn
```sh
# workspace darknet2ncnn
cd ncnn
mkdir build
cd build
cmake ..
make -j8
make install
cd ../../
```
6. Build darknet2ncnn , convert_verify and libdarknet2ncnn.a
```sh
# workspace darknet2ncnn
make -j8
```
7. Convert and verify
```sh
# workspace darknet2ncnn
make cifar
make tiny-yolov3.net

```
8. Build example
```sh
# workspace darknet2ncnn
cd example
make -j2
```
10. Run classifier
```sh
# workspace example
make cifar.cifar
```
11. Run Yolo
```sh
# workspace example
 make yolov3-tiny.coco
```
12. Build benchmark
```sh
# workspace darknet2ncnn
cd benchmark
make 
```
13. Run benchmark
- Firefly RK3399 thread2
```
firefly@firefly:~/project/darknet2ncnn/benchmark$ ./benchdarknet 10  2 &
[1] 4556
loop_count = 10
num_threads = 2
powersave = 0
firefly@firefly:~/project/darknet2ncnn/benchmark$ taskset -pc 4,5 4556
pid 4556's current affinity list: 0-5
pid 4556's new affinity list: 4,5         
           cifar  min =   85.09  max =   89.15  avg =   85.81
         alexnet  min =  218.38  max =  220.96  avg =  218.88
         darknet  min =   88.38  max =   88.95  avg =   88.63
       darknet19  min =  330.55  max =  337.12  avg =  333.64
       darknet53  min =  874.69  max =  920.99  avg =  897.19
     densenet201  min =  678.99  max =  684.97  avg =  681.38
      extraction  min =  332.78  max =  340.54  avg =  334.98
        resnet18  min =  238.93  max =  245.66  avg =  240.32
        resnet34  min =  398.92  max =  404.93  avg =  402.18
        resnet50  min =  545.39  max =  558.67  avg =  551.90
       resnet101  min =  948.88  max =  960.51  avg =  952.99
       resnet152  min = 1350.78  max = 1373.51  avg = 1363.40
       resnext50  min =  660.55  max =  698.07  avg =  669.49
resnext101-32x4d  min = 1219.80  max = 1232.07  avg = 1227.58
resnext152-32x4d  min = 1788.03  max = 1798.79  avg = 1795.48
          vgg-16  min =  883.33  max =  903.98  avg =  895.03
     yolov1-tiny  min =  222.40  max =  227.51  avg =  224.67
     yolov2-tiny  min =  250.54  max =  259.84  avg =  252.38
     yolov3-tiny  min =  240.80  max =  249.98  avg =  245.08

```
- Firefly RK3399 thread4
```
firefly@firefly:~/project/darknet2ncnn/benchmark$ ./benchdarknet 10  4 &
[1] 4663 
loop_count = 10
num_threads = 4
powersave = 0
firefly@firefly:~/project/darknet2ncnn/benchmark$ taskset -pc 0-3 4663
pid 4663's current affinity list: 0-5
pid 4663's new affinity list: 0-3        
           cifar  min =   96.51  max =  108.22  avg =  100.60
         alexnet  min =  411.38  max =  432.00  avg =  420.11
         darknet  min =  101.89  max =  119.73  avg =  106.46
       darknet19  min =  421.46  max =  453.59  avg =  433.74
       darknet53  min = 1375.30  max = 1492.79  avg = 1406.82
     densenet201  min = 1154.26  max = 1343.53  avg = 1218.28
      extraction  min =  399.31  max =  460.01  avg =  428.17
        resnet18  min =  317.70  max =  376.89  avg =  338.93
        resnet34  min =  567.30  max =  604.44  avg =  580.65
        resnet50  min =  838.94  max =  978.21  avg =  925.14
       resnet101  min = 1562.60  max = 1736.91  avg = 1642.27
       resnet152  min = 2250.32  max = 2394.38  avg = 2311.42
       resnext50  min =  993.34  max = 1210.04  avg = 1093.05
resnext101-32x4d  min = 2207.74  max = 2366.66  avg = 2281.82
resnext152-32x4d  min = 3139.89  max = 3372.58  avg = 3282.99
          vgg-16  min = 1259.17  max = 1359.55  avg = 1300.04
     yolov1-tiny  min =  272.31  max =  330.71  avg =  295.98
     yolov2-tiny  min =  314.25  max =  352.12  avg =  329.02
     yolov3-tiny  min =  300.28  max =  349.13  avg =  322.54
```

# Support network(Zoo)

## Cifar
1. cifar

## ImageNet
1. alexnet 
2. darknet
3. darknet19
4. darknet53
5. densenet201
6. extraction
7. resnet18
8. resnet34
9. resnet50
10. resnet101
11. resnet152
12. resnext50
13. resnext101-32x4d
14. resnext152-32x4d
15. vgg-16

## YOLO
1. yolov1-tiny
2. yolov2-tiny
3. yolov2
4. yolov3-tiny
5. yolov3
6. yolov3-spp

# Benchmark

Time： ms

Network | i7-7700K 4.20GHz 8thread | IMX6Q,Topeet 4thead | Firefly rk3399 2thread | Firefly rk3399 4thread
---------|----------|---------|---------|---------
cifar | 62 | 433 | 85 | 100
alexnet | 92 | 1297 | 218 | 420
darknet | 28 | 458 | 88 | 106
darknet19 | 202 | 2356 | 333 | 433
darknet53 | 683 | 7853 | 897 | 1406
densenet201 |218  | 6592 | 681 | 1218
extraction | 244 | 2407 | 334 | 428
resnet18 | 174 | 1563 | 240 | 338
resnet34 | 311 | 2605 | 402 | 580
resnet50 | 276 | 5718 | 551 | 925
resnet101 | 492 | 10450 | 952 | 1642
resnet152 | 704 | 15093 | 1363 | 2311
resnext50 | 169 | 8084  | 669  | 1093
resnext101-32x4d | 296 | 15334 | 1227 | 2281
resnext152-32x4d | 438 | 22843 | 1795 | 3282
vgg-16 | 884 | 7512 | 895 | 1300
yolov1-tiny| 98 | 1283 | 224 | 295
yolov2-tiny| 155 | 1741 | 252 | 329
yolov2| 1846 | Out of memofy| Out of memofy | Out of memofy
yolov3-tiny| 159 | 1791 | 245 | 322
yolov3| 5198 | Out of memofy| Out of memofy | Out of memofy
yolov3-spp| 5702 | Out of memofy | Out of memofy | Out of memofy
