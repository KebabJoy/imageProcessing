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
    VideoProcessor(std::string _path, std::string _filename, int thres = 180){
        path = _path;
        filename = _filename;
        cap.open(path);
        if(cap.isOpened()){
            framesCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
            buildInitialFrames();
        }
        threshold = thres;
    }
    
    void call(){
        colorReduction();
        for (int i = 0; i < 3; ++i) {
            double bright_pixels = 0;
            for (int j = 0; j < grayscaleFrames[i].rows; j++) {
                for (int k = 0; k < grayscaleFrames[i].cols; k++) {
                    if (grayscaleFrames[i].at<uint8_t>(j, k) >= 155) {
                        bright_pixels++;
                    }
                }
            }
//            std::cout << bright_pixels / (grayscaleFrames[i].rows * grayscaleFrames[i].cols) * 100.0 << '\n';
            if(bright_pixels / (grayscaleFrames[i].rows * grayscaleFrames[i].cols) * 100 > 50){
                cv::threshold(grayscaleFrames[i], binarizedFrames[i], threshold, 255, cv::THRESH_BINARY);
            }
            else{
                cv::adaptiveThreshold(grayscaleFrames[i], binarizedFrames[i], 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
            }
            
            
            double white_pixels_percentage = 0;
            for (int j = 0; j < binarizedFrames[i].rows; j++) {
                for (int k = 0; k < binarizedFrames[i].cols; k++) {
                    if (binarizedFrames[i].at<uint8_t>(j, k) == 255) {
                        white_pixels_percentage++;
                    }
                }
            }
            white_pixels_percentage = white_pixels_percentage / (binarizedFrames[i].rows * binarizedFrames[i].cols) * 100;
            if (white_pixels_percentage > 50) {
                cv::bitwise_not(binarizedFrames[i], binarizedFrames[i]);
            }
            
            cv::imwrite("./binarized/" + filename + "/" + std::to_string(i+1) + ".png", binarizedFrames[i]);
            
            cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6, 12));

            cv::morphologyEx(binarizedFrames[i], morphImages[i], cv::MORPH_CLOSE, structuring_element);
            cv::morphologyEx(morphImages[i], morphImages[i], cv::MORPH_OPEN, structuring_element);

            structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(100, 50));
            cv::morphologyEx(morphImages[i], morphImages[i], cv::MORPH_CLOSE, structuring_element);

            cv::imwrite("./morph/" + filename + "/" + std::to_string(i + 1) + ".png", morphImages[i]); //saving bin img

            //finding the main connected component
            //arrays for connecteedComponentsWithStats output
            cv::Mat labeledImage(morphImages[i].size(), CV_32S), stats, centroids;
            //labeling connected components and gathering stats
            
            int nLabels = cv::connectedComponentsWithStats(morphImages[i], labeledImage, stats, centroids, 8, CV_32S);

            //finding main connected components by area, providing it's the biggest except background
            int max_area = 0, max_label = 1;

            for (int j = 1; j < nLabels; j++) {
                if (max_area < stats.at<int>(j, cv::CC_STAT_AREA)) {
                    max_area = stats.at<int>(j, cv::CC_STAT_AREA);
                    max_label = j;
                }
            }
            
            //creating vector of colors
            std::vector<uint8_t> colors(nLabels);
            for (int j = 0; j < nLabels; j++) {
                colors[j] = 0;
            }
            colors[max_label] = 255;
            
            cv::Mat frameCC = initialFrames[i].clone();
            cv::cvtColor(frameCC, frameCC, cv::COLOR_BGR2GRAY);
            frameCC = 0;

            //leaving the main component only visible
            for (int j = 0; j < labeledImage.rows; j++) {
                for (int k = 0; k < labeledImage.cols; k++) {
                    int label = labeledImage.at<int>(j, k);
                    uint8_t& pixel = frameCC.at<uint8_t>(j, k);
                    pixel = colors[label];
                }
            }
            
            cv::imwrite("frames/" + filename + "_CC_" + std::to_string(i + 1) + ".png", frameCC); //saving img with CCs

            //estimating the quality of the mask, we've got
            cv::Mat standard_mask = cv::imread("../data/masks_lab04/" + filename + "/" + std::to_string(i + 1) + ".png").clone();
            cv::cvtColor(standard_mask, standard_mask, cv::COLOR_BGR2GRAY);
            std::cout << "Quality of " + filename + " " << i + 1 << ": " << estimate_quality(frameCC, standard_mask) << std::endl;

            //creating an image with mask overlaying original
            cv::Mat mask_over_original(frameCC.size(), CV_8UC3);
            mask_over_original = 0;
            //4 cases - 4 colors
            for (int j = 0; j < initialFrames[i].rows; j++) {
                for (int k = 0; k < initialFrames[i].cols; k++) {
                    if ((standard_mask.at<uint8_t>(j, k) == 0) && (frameCC.at<uint8_t>(j, k) == 0)) {
                        mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(0, 0, 0);
                    }
                    else if ((standard_mask.at<uint8_t>(j, k) == 0) && (frameCC.at<uint8_t>(j, k) == 255)) {
                        mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(0, 0, 255);
                    }
                    else if ((standard_mask.at<uint8_t>(j, k) == 255) && (frameCC.at<uint8_t>(j, k) == 0)) {
                        mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(255, 0, 0);
                    }
                    else if ((standard_mask.at<uint8_t>(j, k) == 255) && (frameCC.at<uint8_t>(j, k) == 255)) {
                        mask_over_original.at<cv::Vec3b>(j, k) = cv::Vec3b(255, 255, 255);
                    }
                }
            }
            //combining with the original
            cv::addWeighted(initialFrames[i], 0.5, mask_over_original, 0.5, 0.0, mask_over_original);
            
            cv::imwrite("./morph/" + filename + "/" + std::to_string(i+1) + ".png", morphImages[i]);
            cv::imwrite("./mask/" + filename + "/" + std::to_string(i+1) + ".png", mask_over_original);
            cv::imshow("f", mask_over_original);
            cv::waitKey(0);
            
        }
    }
    
private:
    double estimate_quality(cv::Mat mask, cv::Mat standard_mask) {
        double quality = 0;
        
        for (int i = 0; i < mask.rows; i++) {
            for (int j = 0; j < mask.cols; j++) {
                if (mask.at<uint8_t>(i, j) == standard_mask.at<uint8_t>(i, j)) {
                    quality++;
                }
            }
        }
        quality = quality / (mask.rows * mask.cols) * 100;

        return quality;
    }
    
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
    cv::Mat morphImages[3];
    cv::Mat grayscaleFrames[3];
    cv::Mat binarizedFrames[3];
    cv::VideoCapture cap;
    int framesCount;
    int threshold;
};

int main() {
    VideoProcessor v1("../data/1.mp4", "1");
    VideoProcessor v2("../data/2.mp4", "2");
    VideoProcessor v3("../data/3.mp4", "3");
    VideoProcessor v4("../data/4.mp4", "4");
    VideoProcessor v5("../data/5.mp4", "5");
    v1.call();
    v2.call();
    v3.call();
    v4.call();
    v5.call();
}
