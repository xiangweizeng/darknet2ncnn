/**
 * @File   : yolo_v1_detection.h
 * @Author : damone (damonexw@gmail.com)
 * @Link   : 
 * @Date   : 11/2/2018, 2:20:29 PM
 */

#ifndef _YOLO_V1_DETECTION_H
#define _YOLO_V1_DETECTION_H

#include "layer.h"

namespace ncnn
{

::ncnn::Layer *Yolov1Detection_layer_creator();
class Yolov1Detection : public Layer
{
public:
  Yolov1Detection();
  ~Yolov1Detection();

  virtual int load_param(const ParamDict &pd);
  virtual int forward_inplace(Mat &bottom_top_blob, const Option &opt) const;

public:
  int side;
  int classes;
  int box_num;
  int sqrt_enable;
  int softmax_enable;

  float confidence_threshold;
  float nms_threshold;

  ncnn::Layer *softmax;
};

} // namespace ncnn

#endif