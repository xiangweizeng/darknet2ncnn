/**
 * @File   : register_darknet.cpp
 * @Author : damone (damonexw@gmail.com)
 * @Link   :
 * @Date   : 10/30/2018, 4:52:30 PM
 */

#include "darknet2ncnn.h"
#include "layer/darknet_activation.h"
#include "layer/darknet_shortcut.h"
#include "layer/yolov1_detection.h"
#include "layer/yolov3_detection.h"

void register_darknet_layer(ncnn::Net &net) {
  net.register_custom_layer("DarknetActivation", ncnn::DarknetActivation_layer_creator);
  net.register_custom_layer("DarknetShortcut", ncnn::DarknetShortcut_layer_creator);
  net.register_custom_layer("Yolov1Detection", ncnn::Yolov1Detection_layer_creator);
  net.register_custom_layer("Yolov3Detection", ncnn::Yolov3Detection_layer_creator);
}