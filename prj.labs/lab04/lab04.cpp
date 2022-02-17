//
//  main.cpp
//  OpenCVstudying
//
//  Created by Nikita Sidorov on 8.03.2022.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
int min = 0, vmax = 255;
class VideoProcessor{
public:
    VideoProcessor(std::string _path, std::string _filename, std::vector<cv::Rect> rec = {}, int thres = 125){
        path = _path;
        filename = _filename;
        cap.open(path);
        if(cap.isOpened()){
            framesCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
            buildInitialFrames();
        }
        crop = rec;
        threshold = thres;
    }
    
    void call(){
        colorReduction();
        for (int i = 0; i < 3; ++i) {
            cv::cvtColor(initialFrames[i](crop[i]), rectangledImages[i], cv::COLOR_BGR2GRAY);
         
            cv::Mat output(initialFrames[i].rows, initialFrames[i].cols, CV_8UC1, cv::Scalar(0));
            cv::threshold(rectangledImages[i], binarizedFrames[i], threshold, 255, cv::THRESH_BINARY);
            
            binarizedFrames[i].copyTo(output(crop[i]));
            cv::imwrite("./binarized/" + filename + "/" + std::to_string(i+1) + ".png", output);
            
            cv::GaussianBlur(output, output, cv::Size(7,7), 20, 0, cv::BORDER_REFLECT);
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10,10));
            cv::erode(output, output, kernel);
            kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(60,60));
            cv::dilate(output, output, kernel);
            cv::imwrite("./morph/" + filename + "/" + std::to_string(i+1) + ".png", output);
            
            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(output, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
            for (int i = 0; i < contours.size(); ++i) {
                std::vector<std::vector<cv::Point>> conPoly(contours.size());
                float peri = cv::arcLength(contours[i], true);
                cv::approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
                cv::fillPoly(output, conPoly, cv::Scalar(255));
            }
            
            cv::imwrite("./mask/" + filename + "/" + std::to_string(i+1) + ".png", output);
            cv::imshow("f", output);
            cv::waitKey(0);
            
        }
    }
    
private:
    void colorReduction(){
        for (int i = 0; i < 3; ++i) {
            cv::cvtColor(initialFrames[i], grayscaleFrames[i], cv::COLOR_BGR2GRAY);
            cv::imwrite("./grayscale/" + filename + "/" + std::to_string(i+1) + ".png", grayscaleFrames[i]);
        }
    }
    
    void buildInitialFrames() {
        for (int i = 0; i < 3; ++i) {
            cap.set(cv::CAP_PROP_POS_FRAMES, framesCount / 5 * (i + 2));
            cap >> initialFrames[i];
            cv::imwrite("./initial/" + filename + "/" + std::to_string(i+1) + ".png", initialFrames[i]);
        }
    }
    
    std::string path;
    std::string filename;
    cv::Mat initialFrames[3];
    cv::Mat rectangledImages[3];
    cv::Mat grayscaleFrames[3];
    cv::Mat binarizedFrames[3];
    cv::VideoCapture cap;
    std::vector<cv::Rect> crop;
    int framesCount;
    int threshold;
};

int main() {
    std::vector<cv::Rect> r1 = {cv::Rect(11, 419, 417, 203), cv::Rect(7, 273, 472, 238), cv::Rect(13, 424, 433, 213)};
    std::vector<cv::Rect> r2 = {cv::Rect(133, 129, 529, 266), cv::Rect(90, 39, 660, 334), cv::Rect(188, 140, 515, 246)};
    std::vector<cv::Rect> r3 = {cv::Rect(45, 86, 651, 347), cv::Rect(146, 31, 594, 316), cv::Rect(60, 62, 650, 341)};
    std::vector<cv::Rect> r4 = {cv::Rect(85, 121, 545, 294), cv::Rect(21, 104, 573, 307), cv::Rect(87, 100, 576, 304)};
    std::vector<cv::Rect> r5 = {cv::Rect(96, 56, 659, 336), cv::Rect(122, 65, 642, 329), cv::Rect(78, 79, 659, 349)};
    VideoProcessor v1("../data/1.mp4", "1", r1, 125);
    VideoProcessor v2("../data/2.mp4", "2", r2, 170);
    VideoProcessor v3("../data/3.mp4", "3", r3, 130);
    VideoProcessor v4("../data/4.mp4", "4", r4, 150);
    VideoProcessor v5("../data/5.mp4", "5", r5, 100);
    v1.call();
    v2.call();
    v3.call();
    v4.call();
    v5.call();
}
