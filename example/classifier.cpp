// Tencent is pleased to support the open source community by making ncnn
// available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
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
#include <vector>
#include "layer/input.h"
#include "ncnn_tools.h"
#include "darknet2ncnn.h"

static int detect_classifier(char *param, char *bin, char *imagepath, std::vector<float> &cls_scores)
{
  CustomizedNet classifier;
  register_darknet_layer(classifier);
  classifier.load_param(param);
  classifier.load_model(bin);

  ncnn::Input *input = (ncnn::Input *)classifier.get_layer_from_name("data");
  ncnn::Mat in = get_input_mat_from_image(imagepath, input->h, input->w);
  ncnn::Extractor ex = classifier.create_extractor();
  ex.input("data", in);
  ex.set_light_mode(false);

  ncnn::Blob *out_blob = classifier.get_last_layer_output_blob();
  ncnn::Mat out;
  ex.extract(out_blob->name.c_str(), out);

  cls_scores.resize(out.w);
  for (int j = 0; j < out.w; j++)
  {
    cls_scores[j] = out[j];
  }

  return 0;
}

static int print_topk(const std::vector<std::string> &labels,
                      std::vector<float> &cls_scores, int topk)
{
  // partial sort topk with index
  int size = cls_scores.size();
  std::vector< std::pair<float, int> > vec;
  vec.resize(size);
  for (int i = 0; i < size; i++)
  {
    vec[i] = std::make_pair(cls_scores[i], i);
  }

  std::partial_sort(vec.begin(), vec.begin() + topk, vec.end(),
                    std::greater< std::pair<float, int> >());

  // print topk and score
  for (int i = 0; i < topk; i++)
  {
    float score = vec[i].first;
    int index = vec[i].second;
    fprintf(stderr, "%-4d %-32s = %f\n", index, labels[index].c_str(), score);
  }

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

  std::vector<std::string> labels;
  load_classifier_labels(labels, argv[4]);

  std::vector<float> cls_scores;
  detect_classifier(argv[1], argv[2], argv[3], cls_scores);

  print_topk(labels, cls_scores, 5);

  return 0;
}
