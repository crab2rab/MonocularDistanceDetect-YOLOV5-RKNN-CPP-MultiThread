// Copyright (c) 2021 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdio.h>
#include <sys/time.h>
#include <thread>
#include <queue>
#include <vector>
#define _BASETSD_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "rknnPool.hpp"
#include "ThreadPool.hpp"
using std::queue;
using std::time;
using std::time_t;
using std::vector;
int main(int argc, char **argv)
{
  char *model_name = NULL;
  model_name = (char *)argv[1]; // 参数二，模型所在路径
  printf("模型名称:\t%s\n", model_name);

  cv::VideoCapture capture;
  cv::namedWindow("Camera FPS");
  // 打开AHD摄像头，对应需要修改rknnPool.hpp里面图像格式转换，cv::COLOR_YUV2BGR_NV12
  // capture.open("v4l2src device=/dev/video0 ! video/x-raw, format=NV12, width=1920, height=1080, framerate=30/1 ! appsink", cv::CAP_GSTREAMER);
  // 打开USB摄像头，对应需要修改rknnPool.hpp里面图像格式转换，cv::COLOR_RGB2BGR，或者不需要修改通道顺序，看usb摄像头的输出格式
  capture.open("/dev/video0");
  // 打开视频
  // capture.open("./test.mp4");
  // 设置线程数
  int n = 4, frames = 0;
  printf("线程数:\t%d\n", n);
  // 类似于多个rk模型的集合?
  vector<rknn_lite *> rkpool;
  // 线程池
  dpool::ThreadPool pool(n);
  // 线程队列
  queue<std::future<int>> futs;

  //初始化
  for (int i = 0; i < n; i++)
  {
    rknn_lite *ptr = new rknn_lite(model_name, i % 3);
    rkpool.push_back(ptr);
    capture >> ptr->ori_img;
    futs.push(pool.submit(&rknn_lite::interf, &(*ptr)));
  }

  struct timeval time;
  gettimeofday(&time, nullptr);
  auto initTime = time.tv_sec * 1000 + time.tv_usec / 1000;

  gettimeofday(&time, nullptr);
  long tmpTime, lopTime = time.tv_sec * 1000 + time.tv_usec / 1000;

  while (capture.isOpened())
  {
    if (futs.front().get() != 0)
      break;
    futs.pop();
    cv::imshow("Camera FPS", rkpool[frames % n]->ori_img);
    if (cv::waitKey(1) == 'q') // 延时1毫秒,按q键退出
      break;
    if(!capture.read(rkpool[frames % n]->ori_img))
      break;
    futs.push(pool.submit(&rknn_lite::interf, &(*rkpool[frames++ % n])));
    if(frames % 60 == 0){
        gettimeofday(&time, nullptr);
        tmpTime = time.tv_sec * 1000 + time.tv_usec / 1000;
        printf("60帧平均帧率:\t%f帧\n", 60000.0 / (float)(tmpTime - lopTime));
        lopTime = tmpTime;
    }
  }

  gettimeofday(&time, nullptr);
  printf("\n平均帧率:\t%f帧\n", float(frames) / (float)(time.tv_sec * 1000 + time.tv_usec / 1000 - initTime + 0.0001) * 1000.0);

  // 释放剩下的资源
  while (!futs.empty())
  {
    if (futs.front().get())
      break;
    futs.pop();
  }
  for (int i = 0; i < n; i++)
    delete rkpool[i];
  capture.release();
  cv::destroyAllWindows();
  return 0;
}
