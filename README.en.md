# darknet2ncnn

# Introduction
Darknet2ncnn converts the darknet model to the ncnn model, enabling rapid deployment of the darknet network model on the mobile device.
1. Support network layers except local/xor conv, rnn, lstm, gru, crnn and iseg
1. Added all activation operations not directly supported by ncnn, implemented in the layer DarknetActivation
1. Added the implementation of the shortcut layer, implemented in the layer DarknetShortCut
1. Added yolo layer and detection layer implementation, support YOLOV1 and YOLOV3
1. Provides a converted model verification tool, convert_verify, which supports checking the calculation output of each layer of the network, supports convolutional layer parameter checking, and facilitates rapid positioning of problems in model conversion.

# Install&Usage

1. Install opencv-dev, gcc, g++
2. 克隆代码
3. 下载子模块
4. 构建darknet
5. 构建ncnn
6. 构建darknet2ncnn 及convert_verify
7. 转换模型并校验模型
8. 构建example
10. 运行分类模型
11. 运行监测模型
12. 构建benchmark
13. 运行benchmark

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
Network | i7-7700K 4.20GHz 8thread| IMX6Q,Topeet 4thead | Firefly rk3399 2thread | Firefly rk3399 4thread
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