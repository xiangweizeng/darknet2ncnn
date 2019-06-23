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

void register_darknet_layer(ncnn::Net &net)
{
  net.register_custom_layer("DarknetActivation", ncnn::DarknetActivation_layer_creator);
  net.register_custom_layer("DarknetShortcut", ncnn::DarknetShortcut_layer_creator);
  net.register_custom_layer("Yolov1Detection", ncnn::Yolov1Detection_layer_creator);
  net.register_custom_layer("Yolov3Detection", ncnn::Yolov3Detection_layer_creator);
}

DarknetLayerType get_darknet_layer_type_index(std::string layer_type)
{
  if ("DarknetActivation" == layer_type)
    return Darknet_Activition;
  else if ("DarknetShortcut" == layer_type)
    return Darknet_ShortCut;
  else if ("Yolov1Detection" == layer_type)
    return Darknet_Yolov1;
  else if ("Yolov3Detection" == layer_type)
    return Darknet_Yolov3;

  return (DarknetLayerType)ncnn::LayerType::CustomBit;
}

void register_darknet_layer_by_index(ncnn::Net &net)
{
  net.register_custom_layer(Darknet_Activition, ncnn::DarknetActivation_layer_creator);
  net.register_custom_layer(Darknet_ShortCut, ncnn::DarknetShortcut_layer_creator);
  net.register_custom_layer(Darknet_Yolov1, ncnn::Yolov1Detection_layer_creator);
  net.register_custom_layer(Darknet_Yolov3, ncnn::Yolov3Detection_layer_creator);
}
