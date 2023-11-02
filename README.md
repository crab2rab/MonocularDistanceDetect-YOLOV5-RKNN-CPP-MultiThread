# 简介
* 此仓库为c++多线程实现基于yolo5的单目摄像头测距
* RKNN推理基于https://github.com/leafqycc/rknn-cpp-Multithreading
* 单目摄像头测距基于https://github.com/404nofound/Monocular_Distance_Detect
* 主要代码include/rknnPool.hpp include/distanceDetect.hpp
* 摄像头标定以及测距原理，请参考https://blog.csdn.net/qq_41690864/article/details/118863145

# 更新说明


# 使用说明
### 模型转换
  * yolov5工程下export.py转换onnx，直接转，切忌修改batch_size
  * 参考网上方法，rknn-toolkit2进行onnx转rknn

### 演示
  * 系统需安装有**OpenCV**，如果需要读取AHD摄像头还需要安装**gstreamer**
  * 运行build-linux_RK3588.sh
  * 可切换至root用户运行performance.sh定频提高性能和稳定性
  * 编译完成后进入install运行命令./rknn_yolov5_demo **模型所在路径**

### 部署应用
  * 修改include/rknnPool.hpp中的rknn_lite类
  * 修改inclue/rknnPool.hpp中的rknnPool类的构造函数

# 多线程模型帧率测试
* 使用performance.sh进行CPU/NPU定频尽量减少误差

# 补充
* 异常处理尚未完善, 目前仅支持rk3588/rk3588s下的运行
* 摄像头角度标定本人研究了一套方案，可以半自动可视化程序实现精确标定，有偿提供，请私信

# Acknowledgements
* https://github.com/rockchip-linux/rknpu2
* https://github.com/senlinzhan/dpool
* https://github.com/ultralytics/yolov5
* https://github.com/airockchip/rknn_model_zoo
* https://github.com/rockchip-linux/rknn-toolkit2
* https://github.com/leafqycc/rknn-cpp-Multithreading
* https://blog.csdn.net/qq_41690864/article/details/118863145

