/**
 * @File   : ncnn_tools.h
 * @Author : damone (damonexw@gmail.com)
 * @Link   :
 * @Date   : 10/29/2018, 12:33:17 PM
 */

#ifndef _NCNN_TOOLS_H
#define _NCNN_TOOLS_H

#include <stdio.h>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "net.h"

static void print_mat(ncnn::Mat &mat)
{
  for (int c = 0; c < mat.c; c++)
  {
    float *ptr = mat.channel(c);
    for (int h = 0; h < mat.h; h++)
    {
      for (int w = 0; w < mat.w; w++)
      {
        printf("%.2f ", ptr[h * mat.w + w]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

static cv::Mat letter_box_image(const cv::Mat &src, int net_w, int net_h)
{
  int new_w = src.cols;
  int new_h = src.rows;

  if (((float)net_w / src.cols) < ((float)net_h / src.rows))
  {
    new_w = net_w;
    new_h = (src.rows * net_w) / src.cols;
  }
  else
  {
    new_h = net_h;
    new_w = (src.cols * net_h) / src.rows;
  }

  cv::Mat dest(net_w, net_h, CV_8UC3, cv::Scalar(128, 128, 128));
  cv::Mat embed;
  cv::resize(src, embed, cv::Size(new_w, new_h), 0, 0, cv::INTER_LINEAR);
  cv::Mat imageROI = dest(cv::Rect((net_w - new_w) / 2, (net_h - new_h) / 2,
                                   embed.cols, embed.rows));
  embed.copyTo(imageROI);

  return dest;
}

static cv::Mat get_input_opencv_mat_from_image(const char *filename, int net_h,
                                               int net_w)
{
  cv::Mat bgr = cv::imread(filename, CV_LOAD_IMAGE_COLOR);

  if (bgr.empty())
  {
    fprintf(stderr, "cv::imread %s failed\n", filename);
    return bgr;
  }

  cv::Mat letter = letter_box_image(bgr, net_h, net_w);
  return letter;
}

static ncnn::Mat get_input_mat_from_image(const char *filename, int net_h,
                                          int net_w)
{
  cv::Mat letter = get_input_opencv_mat_from_image(filename, net_h, net_w);

  ncnn::Mat darknet_input = ncnn::Mat::from_pixels(
      letter.data, ncnn::Mat::PIXEL_RGB2BGR, net_w, net_h);

  const float norm_vals[3] = {1 / 255.0, 1 / 255.0, 1 / 255.0};
  darknet_input.substract_mean_normalize(0, norm_vals);

  return darknet_input;
}

static std::string trim(const std::string &str, char value = '\n')
{
  std::string::size_type pos = str.find_first_not_of(value);
  if (pos == std::string::npos)
  {
    return str;
  }
  std::string::size_type pos2 = str.find_last_not_of(value);
  if (pos2 != std::string::npos)
  {
    return str.substr(pos, pos2 - pos + 1);
  }
  return str.substr(pos);
}

static void load_classifier_labels(std::vector<std::string> &labels,
                                   char *label_file)
{
  std::ifstream in(label_file);
  if (in)
  {
    std::string label;
    while (std::getline(in, label))
    {
      label = trim(label, '\n');
      label = trim(label, '\r');
      label = trim(label, ' ');
      labels.push_back(label);
    }
  }
}

class CustomizedNet : public ncnn::Net
{
public:
  CustomizedNet() : ncnn::Net(){};

  ncnn::Layer *get_layer_from_name(const char *layer_name)
  {
    int index = find_layer_index_by_name(layer_name);
    if (index < 0)
    {
      return NULL;
    }
    return layers[index];
  }

  ncnn::Blob *get_blob_from_name(const char *blob_name)
  {
    int index = find_blob_index_by_name(blob_name);
    if (index < 0)
    {
      return NULL;
    }
    return &(blobs[index]);
  }

  ncnn::Blob *get_last_layer_output_blob()
  {
    int layer_index = layers.size() - 1;
    if (layer_index < 0)
    {
      return NULL;
    }

    int blob_index = layers[layer_index]->tops[0];
    return &(blobs[blob_index]);
  }
};

#endif