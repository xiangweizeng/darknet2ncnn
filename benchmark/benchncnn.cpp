#include <float.h>
#include <stdio.h>

#ifdef _WIN32
#define NOMINMAX
#include <algorithm>
#include <windows.h> // Sleep()
#else
#include <unistd.h> // sleep()
#endif

#include "benchmark.h"
#include "cpu.h"
#include "net.h"
#include "darknet2ncnn.h"
#include "layer/input.h"

#if NCNN_VULKAN
#include "gpu.h"

class GlobalGpuInstance
{
public:
    GlobalGpuInstance() { ncnn::create_gpu_instance(); }
    ~GlobalGpuInstance() { ncnn::destroy_gpu_instance(); }
};
// initialize vulkan runtime before main()
GlobalGpuInstance g_global_gpu_instance;
#endif // NCNN_VULKAN

namespace ncnn
{

// always return empty weights
class ModelBinFromEmpty : public ModelBin
{
public:
    virtual Mat load(int w, int /*type*/) const { return Mat(w); }
};

class BenchNet : public Net
{
public:
    int load_model()
    {
        // load file
        int ret = 0;

        ModelBinFromEmpty mb;
        for (size_t i = 0; i < layers.size(); i++)
        {
            Layer *layer = layers[i];

            int lret = layer->load_model(mb);
            if (lret != 0)
            {
                fprintf(stderr, "layer load_model %d failed\n", (int)i);
                ret = -1;
                break;
            }

            int cret = layer->create_pipeline(opt);
            if (cret != 0)
            {
                fprintf(stderr, "layer create_pipeline %d failed\n", (int)i);
                ret = -1;
                break;
            }
        }

#if NCNN_VULKAN
        if (opt.use_vulkan_compute)
        {
            upload_model();

            create_pipeline();
        }
#endif // NCNN_VULKAN

        return ret;
    }

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

} // namespace ncnn

static int g_warmup_loop_count = 3;
static int g_loop_count = 4;

static ncnn::Option g_default_option;

static ncnn::UnlockedPoolAllocator g_blob_pool_allocator;
static ncnn::PoolAllocator g_workspace_pool_allocator;

#if NCNN_VULKAN
static ncnn::VulkanDevice* g_vkdev = 0;
static ncnn::VkAllocator* g_blob_vkallocator = 0;
static ncnn::VkAllocator* g_staging_vkallocator = 0;
#endif // NCNN_VULKAN


void network_init(ncnn::BenchNet &net, const char *param)
{
    net.load_param(param);
}

void network_run(ncnn::BenchNet &net)
{
    ncnn::Extractor ex = net.create_extractor();

    ncnn::Input *input = (ncnn::Input *)net.get_layer_from_name("data");
    ncnn::Mat in(input->w, input->h, input->c);
    ex.input("data", in);

    ncnn::Mat out;
    ncnn::Blob *out_blob = net.get_last_layer_output_blob();
    ex.extract(out_blob->name.c_str(), out);
}

void benchmark(const char *param, const char *comment)
{
    ncnn::BenchNet net;
    register_darknet_layer(net);
    network_init(net, param);

    net.load_model();
    g_blob_pool_allocator.clear();
    g_workspace_pool_allocator.clear();

#if NCNN_VULKAN
    if (net.opt.use_vulkan_compute)
    {
        g_blob_vkallocator->clear();
        g_staging_vkallocator->clear();
    }
#endif // NCNN_VULKAN

    // sleep 10 seconds for cooling down SOC  :(
#ifdef _WIN32
    Sleep(10 * 1000);
#else
//     sleep(10);
#endif

    // warm up
    network_run(net);
    network_run(net);
    network_run(net);

    double time_min = DBL_MAX;
    double time_max = -DBL_MAX;
    double time_avg = 0;

    for (int i = 0; i < g_loop_count; i++)
    {
        double start = ncnn::get_current_time();
        network_run(net);
        double end = ncnn::get_current_time();
        double time = end - start;

        time_min = std::min(time_min, time);
        time_max = std::max(time_max, time);
        time_avg += time;
    }

    time_avg /= g_loop_count;

    fprintf(stderr, "%16s  min = %7.2f  max = %7.2f  avg = %7.2f\n", comment, time_min, time_max, time_avg);
}

int main(int argc, char **argv)
{
    int loop_count = 4;
    int num_threads = ncnn::get_cpu_count();
    int powersave = 0;
    int gpu_device = -1;

    if (argc >= 2)
    {
        loop_count = atoi(argv[1]);
    }
    if (argc >= 3)
    {
        num_threads = atoi(argv[2]);
    }
    if (argc >= 4)
    {
        powersave = atoi(argv[3]);
    }
    if (argc >= 5)
    {
        gpu_device = atoi(argv[4]);
    }

    bool use_vulkan_compute = gpu_device != -1;

    g_loop_count = loop_count;

    g_blob_pool_allocator.set_size_compare_ratio(0.0f);
    g_workspace_pool_allocator.set_size_compare_ratio(0.5f);

#if NCNN_VULKAN
    if (use_vulkan_compute)
    {
        g_warmup_loop_count = 10;

        g_vkdev = ncnn::get_gpu_device(gpu_device);

        g_blob_vkallocator = new ncnn::VkUnlockedBlobBufferAllocator(g_vkdev);
        g_staging_vkallocator = new ncnn::VkUnlockedStagingBufferAllocator(g_vkdev);
    }
#endif // NCNN_VULKAN

    // default option
    g_default_option.lightmode = true;
    g_default_option.num_threads = num_threads;
    g_default_option.blob_allocator = &g_blob_pool_allocator;
    g_default_option.workspace_allocator = &g_workspace_pool_allocator;
#if NCNN_VULKAN
    g_default_option.blob_vkallocator = g_blob_vkallocator;
    g_default_option.workspace_vkallocator = g_blob_vkallocator;
    g_default_option.staging_vkallocator = g_staging_vkallocator;
#endif // NCNN_VULKAN
    g_default_option.use_winograd_convolution = true;
    g_default_option.use_sgemm_convolution = true;
    g_default_option.use_int8_inference = true;
    g_default_option.use_vulkan_compute = use_vulkan_compute;
    g_default_option.use_fp16_packed = true;
    g_default_option.use_fp16_storage = true;
    g_default_option.use_fp16_arithmetic = true;
    g_default_option.use_int8_storage = true;
    g_default_option.use_int8_arithmetic = true;

    ncnn::set_cpu_powersave(powersave);

    ncnn::set_omp_dynamic(0);
    ncnn::set_omp_num_threads(num_threads);

    fprintf(stderr, "loop_count = %d\n", g_loop_count);
    fprintf(stderr, "num_threads = %d\n", num_threads);
    fprintf(stderr, "powersave = %d\n", ncnn::get_cpu_powersave());
    fprintf(stderr, "gpu_device = %d\n", gpu_device);

    benchmark("cifar.param", "cifar");
    benchmark("alexnet.param", "alexnet");
    benchmark("darknet.param", "darknet");
    benchmark("darknet19.param", "darknet19");
    benchmark("darknet53.param", "darknet53");
    benchmark("densenet201.param", "densenet201");
    benchmark("extraction.param", "extraction");
    benchmark("resnet18.param", "resnet18");
    benchmark("resnet34.param", "resnet34");
    benchmark("resnet50.param", "resnet50");
    benchmark("resnet101.param", "resnet101");
    benchmark("resnet152.param", "resnet152");
    benchmark("resnext50.param", "resnext50");
    benchmark("resnext101-32x4d.param", "resnext101-32x4d");
    benchmark("resnext152-32x4d.param", "resnext152-32x4d");
    benchmark("vgg-16.param", "vgg-16");*/
    benchmark("yolov1-tiny.param", "yolov1-tiny");
    benchmark("yolov2-tiny.param", "yolov2-tiny");
    //benchmark("yolov2.param", "yolov2");
    benchmark("yolov3-tiny.param", "yolov3-tiny");
    //benchmark("yolov3.param", "yolov3");
    //benchmark("yolov3-spp.param", "yolov3-spp");

#if NCNN_VULKAN
    delete g_blob_vkallocator;
    delete g_staging_vkallocator;
#endif // NCNN_VULKAN
    return 0;
}
