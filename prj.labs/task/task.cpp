//
//  task.cpp
//  OpenCVstudying
//
//  Created by Nikita Sidorov on 16.05.2022.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
    cv::Mat img(300, 450, CV_32FC1);
    std::vector <std::vector<float>> color = { {0, 127, 255}, {255, 0, 127} };
    int radius = 75;
    int width = 150;
    cv::Rect2d rect(0, 0, 150, 150);
    
    for (int i = 0; i < 2; i++) {
        rect.y = i * width;
        for (int j = 0; j < 3; j++) {
            rect.x = j * width;
            cv::Mat tmp = img(rect);
            tmp = color[i][j] / 255;
            cv::circle(img, cv::Point(rect.x + radius, rect.y + radius), 55, color[1 - i][j] / 255, -1);
        }
    }

    cv::imshow("img", img);

    cv::Mat I1(2, 2, CV_32FC1);
    I1 = 0;
    I1.at<float>(0, 1) = 1;
    I1.at<float>(1, 0) = -1;
    cv::Mat filter1;
    cv::filter2D(img, filter1, -1, I1, cv::Point(0, 0));

    cv::Mat I2(2, 2, CV_32FC1);
    I2 = 0;
    I2.at<float>(1, 0) = 1;
    I2.at<float>(0, 1) = -1;
    cv::Mat filter2;
    cv::filter2D(img, filter2, -1, I2, cv::Point(0, 0));

    cv::Mat middle(300, 450, CV_32FC1);
    for (int i = 0; i < middle.rows; i++) {
        for (int j = 0; j < middle.cols; j++) {
            middle.at<float>(i, j) = filter1.at<float>(i, j) * filter1.at<float>(i, j) + filter2.at<float>(i, j) * filter2.at<float>(i, j);
        }
    }

    for (int i = 0; i < middle.rows; i++) {
        for (int j = 0; j < middle.cols; j++) {
            middle.at<float>(i, j) = std::sqrt(middle.at<float>(i, j));
        }
    }

    filter1 = (filter1 + 1) / 2;
    cv::imshow("filter1", filter1);
    cv::imwrite("filter1.png", filter1);

    filter2 = (filter2 + 1) / 2;
    cv::imshow("filter2", filter2);
    cv::imwrite("filter2.png", filter2);

    cv::imshow("middle", middle);
    cv::imwrite("middle.png", middle);

    cv::waitKey(0);
}

