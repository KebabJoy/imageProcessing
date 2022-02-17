//
//  main.cpp
//  OpenCVstudying
//
//  Created by Nikita Sidorov on 18.02.2022.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

const float Pi = acos(-1);

float f(float x){
    return sqrt(Pi*x)/1.8;
}

cv::Mat genPlot(){
    int h = 512;
    int w = 512;
    cv::Mat ans(h, w, CV_8UC1, 255);
    for (int i = 0; i < w; ++i) {
        ans.at<uchar>(h - 1 - (h-1)*f(i / (w - 1.0)), i) = 0;
    }
    cv::line(ans,
            cv::Point(0, 0),
            cv::Point(0, w),
            cv::Scalar(0, 0, 0), 2, 8, 0);
    cv::line(ans,
            cv::Point(0, w),
            cv::Point(w, w),
            cv::Scalar(0, 0, 0), 2, 8, 0);
    return ans;
}

int main() {
    const std::string path = "../data/cross_0256x0256.png";
    cv::Mat cross_0256x0256 = cv::imread(path);
    cv::Mat cross_0256x0256_lut = cross_0256x0256.clone();
    cv::Mat lookUpTable(1, 256, CV_8UC1), f_plot = genPlot(), grayscale, grayscale_lut;
    cv::cvtColor(cross_0256x0256, grayscale, cv::COLOR_BGR2GRAY);
    uchar* lut  = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
        lut[i] = cv::saturate_cast<uchar>(255 * f(i / 255.0));
    
    cv::LUT(grayscale, lookUpTable, grayscale_lut);
    cv::LUT(cross_0256x0256, lookUpTable, cross_0256x0256_lut);
    cv::imshow("cross_0256x0256", cross_0256x0256);
    cv::imshow("cross_0256x0256_LUT", cross_0256x0256_lut);
    cv::imshow("grayscale", grayscale);
    cv::imshow("grayscale_lut", grayscale_lut);
    cv::imshow("plot", f_plot);
    cv::imwrite("./lut.png", cross_0256x0256_lut);
    cv::imwrite("./f_plot.png", f_plot);
    cv::imwrite("./grayscale.png", grayscale);
    cv::imwrite("./grayscale_lut.png", grayscale_lut);
    cv::waitKey(0);
}
