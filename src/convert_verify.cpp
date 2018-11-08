/**
 * @File   : convert_verify.cpp
 * @Author : damone (damonexw@gmail.com)
 * @Link   :
 * @Date   : 10/30/2018, 9:40:30 AM
 */
#include <algorithm>
#include "darknet2ncnn.h"
#include "darknet_tools.h"
#include "layer/batchnorm.h"
#include "layer/convolution.h"
#include "ncnn_tools.h"
#include "thread_tools.h"
#include "object_detection.h"

float get_layer_output_diff(ncnn::Mat &darknet_out, ncnn::Mat &ncnn_out)
{
  int feature_size = ncnn_out.h * ncnn_out.w;
  float *darknet_ptr = darknet_out;
  float diff = 0;
  for (int c = 0; c < ncnn_out.c; c++)
  {
    float *ncnn_ptr = ncnn_out.channel(c);
    for (int i = 0; i < feature_size; i++)
    {
      diff += fabs((*ncnn_ptr) - (*darknet_ptr));
      ncnn_ptr++;
      darknet_ptr++;
    }
  }
  return diff;
}

void print_layer_output_diff(ncnn::Mat &darknet_out, ncnn::Mat &ncnn_out)
{
  int feature_size = ncnn_out.h * ncnn_out.w;
  float *darknet_ptr = darknet_out;
  for (int c = 0; c < ncnn_out.c; c++)
  {
    float *ncnn_ptr = ncnn_out.channel(c);
    for (int i = 0; i < feature_size; i++)
    {
      float diff = fabs((*ncnn_ptr) - (*darknet_ptr));
      printf("[%.2f %.2f] (%.2f) ", *ncnn_ptr, *darknet_ptr, diff);
      ncnn_ptr++;
      darknet_ptr++;
    }
    printf("\n");
  }
}

void print_conv_weights(network *net, int i, CustomizedNet &classifier)
{
  layer conv = net->layers[i];
  char name[256] = {0};
  sprintf(name, "conv_%d", i);

  ncnn::Mat darknet_weights(conv.nweights, conv.weights);
  ncnn::Convolution *ncnn_conv =
      (ncnn::Convolution *)classifier.get_layer_from_name(name);
  float diff = get_layer_output_diff(darknet_weights, ncnn_conv->weight_data);
  printf("%s : weights diff : %f\n", name, diff);

  if (conv.batch_normalize)
  {
    memset(name, 0, 256);
    sprintf(name, "conv_%d_batch_norm", i);
    ncnn::BatchNorm *ncnn_batch =
        (ncnn::BatchNorm *)classifier.get_layer_from_name(name);

    ncnn::Mat darknet_scales(conv.n, conv.scales);
    diff = get_layer_output_diff(darknet_scales, ncnn_batch->slope_data);
    printf("%s : slope diff : %f\n", name, diff);

    ncnn::Mat darknet_means(conv.n, conv.rolling_mean);
    diff = get_layer_output_diff(darknet_means, ncnn_batch->mean_data);
    printf("%s : mean diff : %f\n", name, diff);

    ncnn::Mat darknet_variance(conv.n, conv.rolling_variance);
    diff = get_layer_output_diff(darknet_variance, ncnn_batch->var_data);
    printf("%s : variance diff : %f\n", name, diff);

    ncnn::Mat darknet_biases(conv.n, conv.biases);
    diff = get_layer_output_diff(darknet_biases, ncnn_batch->bias_data);
    printf("%s : biases diff : %f\n", name, diff);
  }
  else
  {
    ncnn::Mat darknet_biases(conv.n, conv.biases);
    diff = get_layer_output_diff(darknet_biases, ncnn_conv->bias_data);
    printf("%s : biases diff : %f\n", name, diff);
  }
}

void verify_network(char *cfgfile, char *weightfile, char *ncnn_param,
                    char *ncnn_modelbin, char *imagepath)
{
  /**
   * load darknet
   */
  network *net_p = load_network(cfgfile, weightfile, 0);
  set_batch_network(net_p, 1);
  net_p->truth = 0;
  net_p->train = 0;
  net_p->delta = 0;

  /**
   * load ncnn
   */
  CustomizedNet classifier;
  register_darknet_layer(classifier);
  classifier.load_param(ncnn_param);
  classifier.load_model(ncnn_modelbin);

  /**
   * prepare input for darknet
   */
  image darknet_input;
  if (NULL == imagepath)
  {
    darknet_input = make_image(net_p->w, net_p->h, net_p->c);
    fill_image(darknet_input, 0.5f);
  }
  else
  {
    darknet_input = get_input_data_from_image(imagepath, net_p->w, net_p->h);
  }
  net_p->input = darknet_input.data;

  /**
   *  prepare input for ncnn
   */
  ncnn::Mat in;
  if (NULL == imagepath)
  {
    in = ncnn::Mat(net_p->w, net_p->h, net_p->c);
    in.fill(0.5f);
  }
  else
  {
    in = get_input_mat_from_image(imagepath, net_p->h, net_p->w);
  }

  ncnn::Extractor ex = classifier.create_extractor();
  ex.set_light_mode(false);
  ex.input("data", in);

  /**
   * Run all operation from 0 to n-1
   */
  printf("\nStart run all operation:\n");
  for (int i = 0; i < net_p->n; ++i)
  {
    net_p->index = i;
    layer l = net_p->layers[i];
    if (l.type == CONVOLUTIONAL)
    {
      print_conv_weights(net_p, i, classifier);
    }

    l.forward(l, *net_p);
    net_p->input = l.output;
    ncnn::Mat darknet_out(l.outputs, l.output);

    if (l.type == REGION || (l.type == YOLO))
    {
      continue;
    }

    std::string blob_name = get_layer_no_split_output_blob_name(l, i);
    ncnn::Mat ncnn_out;
    ex.extract(blob_name.c_str(), ncnn_out);

    float diff = get_layer_output_diff(darknet_out, ncnn_out);
    printf("Layer: %d, Blob : %s, Total Diff %f Avg Diff: %f\n", i, blob_name.c_str(), diff, diff / l.outputs);

    /*
    if (diff / l.outputs > 1)
    {
      print_layer_output_diff(darknet_out, ncnn_out);
      break;
    }

    if (i == net_p->n - 1)
    {
      printf("Darknet output:\n");
      print_mat(darknet_out);
      printf("Ncnn output:\n");
      print_mat(ncnn_out);
    }*/
  }

  /**
   * Free data for darknet
   */
  free_image(darknet_input);
  net_p->input = NULL;
  free_network(net_p);
}

int main(int argc, char *argv[])
{
  if (argc < 5)
  {
    printf(
        "Usage : %s [darknet.cfg] [darknet.weights] [ncnn.param] [ncnn.bin] "
        "[test.image]",
        argv[0]);
    return -1;
  }

  char *image = NULL;
  if (argc >= 6)
  {
    image = argv[5];
  }

  verify_network(argv[1], argv[2], argv[3], argv[4], image);

  return 0;
}