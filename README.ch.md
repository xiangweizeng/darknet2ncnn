# darknet2ncnn

# 简介

darknet2ncnn将darknet 模型转换为ncnn模型，实现darknet网络模型在移动端的快速部署
1. 除 local/xor conv, rnn, lstm, gru, crnn及iseg外，均提供支持
1. 自定义添加了所有ncnn不直接支持的activation操作，实现位于层DarknetActivation
1. 自定义添加了shortcut层的实现，实现位于层DarknetShortCut
1. 自定义添加了yolo层及detection层的实现，支持YOLOV1及YOLOV3
1. 提供了转换后的模型校验工具，convert_verify,支持检验每一层网络的计算输出，支持卷积层参数检查，方便快速定位模型转换中出现的问题

# 安装及使用

1. 安装 opencv-dev, gcc, g++, make
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

# 支持的网络模型(Zoo)

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

# 性能评估
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