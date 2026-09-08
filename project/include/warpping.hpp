#ifndef WARPPING_H
#define WARPPING_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

// informations about the detected cross
struct CrossInfo {
    Point location;
    bool detection_ok;
    double confidence;
};

CrossInfo detectCross(const Mat& input_img, const Mat& template_img, const Mat& mask);

bool isCrossInExpectedCorner(Point2f cross_pos, int img_width, int img_height, bool is_top_right);

vector<CrossInfo> detectTwoCrosses(const Mat& input_img, const Mat& template_img);

Mat wrappImageWithCrosses(const Mat& input_img, const vector<CrossInfo>& corners, const Mat& template_img);

Mat drawDetections(const Mat& input_img, const vector<CrossInfo>& corners, const Mat& template_img);

int warpping(string input_folder, string template_path, string output_folder, string wrapp_folder);

#endif //WARPPING_H