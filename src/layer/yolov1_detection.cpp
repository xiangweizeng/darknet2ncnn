/**
 * @File   : yolov1_detection.cpp
 * @Author : damone (damonexw@gmail.com)
 * @Link   : 
 * @Date   : 11/2/2018, 2:31:01 PM
 */

#include <algorithm>
#include <math.h>
#include "layer_type.h"
#include "object_detection.h"
#include "yolov1_detection.h"

namespace ncnn
{

DEFINE_LAYER_CREATOR(Yolov1Detection)

Yolov1Detection::Yolov1Detection()
{
    one_blob_only = true;
    support_inplace = true;

    softmax = ncnn::create_layer(ncnn::LayerType::Softmax);
    ncnn::ParamDict pd;
    pd.set(0, 1);
    softmax->load_param(pd);
}

Yolov1Detection::~Yolov1Detection()
{
    if (NULL != softmax)
    {
        delete softmax;
        softmax = NULL;
    }
}

int Yolov1Detection::load_param(const ParamDict &pd)
{
    side = pd.get(0, 7);
    classes = pd.get(1, 20);
    box_num = pd.get(2, 5);
    sqrt_enable = pd.get(3, 1);
    softmax_enable = pd.get(4, 1);
    confidence_threshold = pd.get(5, 0.25f);
    nms_threshold = pd.get(6, 0.45f);

    return 0;
}

int Yolov1Detection::forward_inplace(Mat &bottom_top_blob, const Option &opt) const
{
    int size = side * side;
    int map_total = size * (box_num * 5 + classes);
    if (bottom_top_blob.w < map_total)
        return -1;

    if (softmax_enable)
    {
        Mat classes_scores = bottom_top_blob.range(0, size * classes);
        classes_scores.reshape(classes, size);
        softmax->forward_inplace(classes_scores, opt);
    }

    ObjectsManager objects(size * box_num / 20);
    const float *classes_ptr = bottom_top_blob;
    const float *scales_ptr = classes_ptr + size * classes;
    const float *boxes_ptr = scales_ptr + size * box_num;

    #pragma omp parallel for num_threads(opt.num_threads)
    for (int i = 0; i < size; i++)
    {
        int x_offset = i % side;
        int y_offset = i / side;

        for (int n = 0; n < box_num; n++)
        {

            float w = (sqrt_enable == 0) * boxes_ptr[2] + (sqrt_enable != 0) * boxes_ptr[2] * boxes_ptr[2];
            float h = (sqrt_enable == 0) * boxes_ptr[3] + (sqrt_enable != 0) * boxes_ptr[3] * boxes_ptr[3];
            float xmin = (boxes_ptr[0] + x_offset) / side - w * 0.5f;
            float ymin = (boxes_ptr[1] + y_offset) / side - h * 0.5f;
            float xmax = xmin + w;
            float ymax = ymin + h;
            boxes_ptr += 4;

            float scale = scales_ptr[0];
            scales_ptr++;

            for (int j = 0; j < classes; j++)
            {
                float prob = scale * classes_ptr[j];
                if (prob > confidence_threshold)
                {
                    ObjectBox object = {
                        .xmin = xmin,
                        .ymin = ymin,
                        .xmax = xmax,
                        .ymax = ymax,
                        .prob = prob,
                        .classid = j,
                    };
                    objects.add_new_object_box(object);
                }
            }
        }

        classes_ptr += classes;
    }

    std::vector<ObjectBox> detected_objects;
    objects.do_objects_nms(detected_objects, nms_threshold, confidence_threshold);

    bottom_top_blob.create(6, detected_objects.size(), 4u, opt.blob_allocator);
    if (bottom_top_blob.empty())
        return -100;

    for (int i = 0; i < detected_objects.size(); i++)
    {
        const ObjectBox &r = detected_objects[i];
        float *outptr = bottom_top_blob.row(i);

        outptr[0] = r.classid + 1; // 0 reserve for background
        outptr[1] = r.prob;
        outptr[2] = r.xmin;
        outptr[3] = r.ymin;
        outptr[4] = r.xmax;
        outptr[5] = r.ymax;
    }

    return 0;
}

} // namespace ncnn
