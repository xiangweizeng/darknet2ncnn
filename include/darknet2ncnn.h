/**
 * @File   : darknet2ncnn.h
 * @Author : damone (damonexw@gmail.com)
 * @Link   :
 * @Date   : 10/30/2018, 4:48:57 PM
 */

#ifndef _DARKNET2NCNN_H
#define _DARKNET2NCNN_H

#include <net.h>

/**
 * @brief register support darknet layer to ncnn
 * 
 * @param net 
 */
void register_darknet_layer(ncnn::Net &net);

#endif