// Tencent is pleased to support the open source community by making ncnn
// available.
//
// Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "darknet2ncnn.h"
#include "layer/input.h"
#include "ncnn_tools.h"
#include "benchmark.h"
#include "darknet2ncnn.h"

struct Object
{
  cv::Rect_<float> rect;
  int label;
  float prob;
};

static void draw_objects(const cv::Mat &bgr, const std::vector<Object> &objects,
                         std::vector<std::string> &labels)
{

  cv::Mat image = bgr.clone();

  for (size_t i = 0; i < objects.size(); i++)
  {
    const Object &obj = objects[i];

    fprintf(stderr, "%-3d[%-16s] = %.5f at %.2f %.2f %.2f x %.2f\n", obj.label,
            labels[obj.label].c_str(), obj.prob, obj.rect.x, obj.rect.y,
            obj.rect.width, obj.rect.height);

    cv::rectangle(image, obj.rect, cv::Scalar(255, 0, 0));

    char text[256];
    sprintf(text, "%s %.1f%%", labels[obj.label].c_str(), obj.prob * 100);

    int baseLine = 0;
    cv::Size label_size =
        cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);

    int x = obj.rect.x;
    int y = obj.rect.y - label_size.height - baseLine;
    if (y < 0)
      y = 0;
    if (x + label_size.width > image.cols)
      x = image.cols - label_size.width;

    cv::rectangle(
        image,
        cv::Rect(cv::Point(x, y),
                 cv::Size(label_size.width, label_size.height + baseLine)),
        cv::Scalar(255, 255, 255), CV_FILLED);

    cv::putText(image, text, cv::Point(x, y + label_size.height),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
  }

  cv::imwrite("test.png", image);
}

static int detect_yolov2(char *param, char *bin, cv::Mat &bgr,
                         std::vector<std::string> &labels)
{
  CustomizedNet yolo;
  register_darknet_layer(yolo);
  yolo.load_param(param);
  yolo.load_model(bin);

  ncnn::Input *input = (ncnn::Input *)yolo.get_layer_from_name("data");
  cv::Mat letter = letter_box_image(bgr, input->h, input->w);

  ncnn::Mat in = ncnn::Mat::from_pixels(letter.data, ncnn::Mat::PIXEL_RGB2BGR,
                                        input->w, input->h);
  const float norm_vals[3] = {1 / 255.0, 1 / 255.0, 1 / 255.0};
  in.substract_mean_normalize(0, norm_vals);

  ncnn::Extractor ex = yolo.create_extractor();
  ex.input("data", in);
  ex.set_light_mode(false);
  ex.set_num_threads(2);

  ncnn::Mat out;
  ncnn::Blob *out_blob = yolo.get_last_layer_output_blob();
  int result = ex.extract(out_blob->name.c_str(), out);
  if (result != 0)
  {
    printf("ncnn error: %d\n", result);
    return result;
  }

  int img_h = letter.rows;
  int img_w = letter.cols;
  std::vector<Object> objects;
  for (int i = 0; i < out.h; i++)
  {
    const float *values = out.row(i);

    Object object;
    object.label = values[0];
    object.prob = values[1];
    object.rect.x = values[2] * img_w;
    object.rect.y = values[3] * img_h;
    object.rect.width = values[4] * img_w - object.rect.x;
    object.rect.height = values[5] * img_h - object.rect.y;

    objects.push_back(object);
  }

  draw_objects(letter, objects, labels);
  return 0;
}

int main(int argc, char **argv)
{
  if (argc != 5)
  {
    fprintf(
        stderr,
        "Usage: %s [ncnn.param] [ncnn.bin] [imagepath] [lable.txt]\n",
        argv[0]);
    return -1;
  }

  const char *imagepath = argv[3];
  cv::Mat m = cv::imread(imagepath, CV_LOAD_IMAGE_COLOR);
  if (m.empty())
  {
    fprintf(stderr, "cv::imread %s failed\n", imagepath);
    return -1;
  }

  std::vector<std::string> labels;
  load_classifier_labels(labels, argv[4]);

  detect_yolov2(argv[1], argv[2], m, labels);

  return 0;
}
