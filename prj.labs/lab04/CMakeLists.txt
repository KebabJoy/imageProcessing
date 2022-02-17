project(lab04)
add_executable(lab04 lab04.cpp lab04.report.md.in.txt)
target_link_libraries(lab04 ${OpenCV_LIBS})

file(READ lab04.cpp cpp_source)
# prepare report md file
configure_file(lab04.report.md.in.txt ${CMAKE_CURRENT_BINARY_DIR}/lab04.report.md @ONLY)
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/lab04.report.md" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
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
    VideoProcessor(std::string _path, std::string _filename, std::vector<cv::Rect> rec = {}){
        framesCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
        path = _path;
        filename = _filename;
        cap.open(path);
        if(cap.isOpened()){
            buildInitialFrames();
        }
        
        
    }
    
    void call(){
        
        for (int i = 0; i < 3; ++i) {
            cv::cvtColor(initialFrames[0](v[i]), rectangledImages[0], cv::COLOR_BGR2GRAY);
            cv::Mat output(initialFrames[0].rows, initialFrames[0].cols, CV_8UC1);
            binarizedFrames[0].copyTo(output(v[i]));
            cv::threshold(rectangledImages[0], binarizedFrames[0], 125, 255, cv::THRESH_BINARY);
             
            cv::imshow("f", output);
            cv::waitKey(0);
        }
        
//        colorReduction();
//        binarize();
    }
    
private:
    void colorReduction(){
        for (int i = 0; i < 3; ++i) {
            cv::cvtColor(initialFrames[i], grayscaleFrames[i], cv::COLOR_BGR2GRAY);
            cv::imwrite("./grayscale_frame_" + filename + std::to_string(i + 1) + ".png", grayscaleFrames[i]);
        }
    }
    
    void binarize(){
        cv::namedWindow("trackBars", cv::WINDOW_NORMAL);
        cv::createTrackbar("lowerbound", "trackBars", &min, 255);
        cv::createTrackbar("upperbound", "trackBars", &vmax, 255);
        for (int i = 0; i < 3; ++i) {
            while(true){
                cv::threshold(grayscaleFrames[i], binarizedFrames[i], min, vmax, cv::THRESH_BINARY);
                cv::imshow("f", binarizedFrames[i]);
                char c = cv::waitKey(0);
                if(c == 's') break;
            }
            
//            cv::imwrite("./binarized_frame_" + filename + std::to_string(i + 1) + ".png", binarizedFrames[i]);
        }
    }
    
    void buildInitialFrames() {
        for (int i = 0; i < 3; ++i) {
            cap.set(cv::CAP_PROP_POS_FRAMES, framesCount * (i + 2) / 5);
            cap >> initialFrames[i];
            cv::imwrite("./initial_frame_" + filename + std::to_string(i + 1) + ".png", initialFrames[i]);
        }
    }
    
    std::string path;
    std::string filename;
    cv::Mat initialFrames[3];
    cv::Mat rectangledImages[3];
    cv::Mat grayscaleFrames[3];
    cv::Mat binarizedFrames[3];
    cv::VideoCapture cap;
    std::vector<cv::Rect> v;
    int framesCount;
};

int main() {
    std::vector<cv::Rect> r1 = {cv::Rect(47, 374, 429, 212), cv::Rect(50, 379, 423, 207), cv::Rect(50, 377, 424, 212)};
    VideoProcessor v1("../data/1.mp4", "first", r1);
    VideoProcessor v2("../data/2.mp4", "second");
    VideoProcessor v3("../data/3.mp4", "third");
    VideoProcessor v4("../data/4.mp4", "fourth");
    VideoProcessor v5("../data/5.mp4", "fifth");
    v1.call();
    v2.call();
    v3.call();
    v4.call();
    v5.call();
}
