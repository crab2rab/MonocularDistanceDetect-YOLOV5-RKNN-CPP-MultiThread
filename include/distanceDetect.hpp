#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>

class DistanceEstimation {
public:
    DistanceEstimation() {
        // Camera image dimensions
        W = 1920;
        H = 1080;
        excel_path = "/home/neardi/Desktop/trash_detect/camera_parameters_ahd_2.xlsx";
        loadCameraParameters(excel_path);
    }

    void loadCameraParameters(const std::string& excel_path) {
        // Load camera intrinsic and extrinsic matrices from the Excel file
        // You can use your preferred method for reading Excel files in C++ here.
        // For simplicity, we'll just hardcode the values.
        // In real code, consider using a library like OpenCV or others to read Excel files.
        K << 1143.294, 0.000, 846.240,
             0.000, 1143.560, 471.587,
             0.000, 0.000, 1.000;

        P << 0.00, 1.00, 0.00, 0.00,
             0.00, 0.00, 1.00, -1.00,
             1.00, 0.00, 0.00, 0.00,
             0.00, 0.00, 0.00, 1.00;
    }

    Eigen::Vector3d objectPointWorldPosition(int u, int v, int w, int h) {
        double u1 = u;
        double v1 = v + h / 2;
        double fx = K(0, 0);
        double fy = K(1, 1);
        // Camera height
        double Height = 0.935;//摄像头安装高度

        // Camera tilt angles
        double angle_a = 0.09007590044305702; //摄像头安装角度，俯仰角，向下为正
        double angle_b = std::atan((v1 - H / 2) / fy);
        double angle_c = angle_b + angle_a;

        double depth = (Height / std::sin(angle_c)) * std::cos(angle_b);

        Eigen::Matrix3d K_inv = K.inverse();
        Eigen::Matrix4d P_inv = P.inverse();
        
        Eigen::Vector3d point_c(u1, v1, 1);
        Eigen::Vector3d c_position = K_inv * (depth * point_c);
        Eigen::Vector4d c_position_homogeneous;
        c_position_homogeneous << c_position, 1.0;

        Eigen::Vector4d w_position = P_inv * c_position_homogeneous;
        return w_position.head(3);
    }

    Eigen::Vector3d distance(const std::vector<int>& kuang) {
        if (kuang.size()) {
            int u = (kuang[0]+kuang[2])/2;
            int v = (kuang[1]+kuang[3])/2;
            int w = kuang[2]-kuang[0];
            int h = kuang[3]-kuang[1];
            Eigen::Vector3d d1 = objectPointWorldPosition(u, v, w, h);
            if (d1(0) <= 0) {
                d1.head(3).setZero();
            }
            return d1.head(3);
        }

        return Eigen::Vector3d::Zero();
    }

private:
    int W, H;
    std::string excel_path;
    Eigen::Matrix3d K;
    Eigen::Matrix4d P;
};
