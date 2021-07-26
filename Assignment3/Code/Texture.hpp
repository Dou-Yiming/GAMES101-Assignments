//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        if (u<0) u=0;
        if (v<0) v=0;
        if (u>1) u=1;
        if (v>1) v=1;
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        if (u<0) u=0;
        if (v<0) v=0;
        if (u>1) u=1;
        if (v>1) v=1;
        auto u_img = u * width;
        auto v_img = (1 - v) * height;

        float l = std::floor(u_img);
        float r = std::min((float)width,std::ceil(u_img));
        float t = std::floor(v_img);
        float b = std::min((float)height,std::ceil(v_img));

        auto C11 = image_data.at<cv::Vec3b>(t, l);
        auto C12 = image_data.at<cv::Vec3b>(t, r);
        auto C21 = image_data.at<cv::Vec3b>(b, l);
        auto C22 = image_data.at<cv::Vec3b>(b, r);

        float ratio_x = (u_img-l)/(r-l);
        float ratio_y = (v_img-t)/(b-t);

        auto C_t = C11*(1-ratio_x)+C12*ratio_x;
        auto C_b = C21*(1-ratio_x)+C22*ratio_x;

        auto color = C_t*(1-ratio_y)+C_b*ratio_y;


        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
