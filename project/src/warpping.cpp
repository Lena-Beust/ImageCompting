// Detect the crosses and redress the images

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <cmath>
#include "warpping.hpp"
#include "retriveSymbol.h"

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

// Detect a cross in the sheet
CrossInfo detectCross(const Mat& input_img, const Mat& template_img, const Mat& mask = Mat()){
    CrossInfo info;
    info.detection_ok = false;
    info.confidence = 0.0;
    info.location = Point(-1, -1);

    Mat img = input_img.clone();
    Mat templ = template_img.clone();

    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    if (result_cols <= 0 || result_rows <= 0){
        cerr << "Error : template is too large" << endl;
        return info;
    }

    Mat result= input_img.clone();
    result.create(result_rows, result_cols, CV_32FC1);

    matchTemplate(img, templ, result, TM_CCOEFF_NORMED);
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    double minVal, maxVal;
    Point minLoc, maxLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    info.location = maxLoc;
    info.confidence = maxVal;
    info.detection_ok = (maxVal >= 0.7);
    return info;
}

// Check if the detected cross is in a corner, if not the detection is ignored
bool isCrossInExpectedCorner(Point2f cross_pos, int img_width, int img_height, bool is_top_right){
    int marge= 550;
    if (is_top_right){ // haut-droit
        return (cross_pos.x > (img_width - marge)) && (cross_pos.y < marge);
    }else{ // bas-gauche
        return (cross_pos.x < marge) && (cross_pos.y > (img_height - marge));
    }
}

// Detect the two crosses that are in the diagonale : 2 areas to check -> Right-up and Left-down
vector<CrossInfo> detectTwoCrosses(const Mat& input_img, const Mat& template_img){
    vector<CrossInfo> crosses;

    int width = input_img.cols;
    int height = input_img.rows;

    // Regions of interest => ignored area (~mask)
    vector<Rect> rois = {
        Rect(width - width / 4, 0, width / 4, height / 4), // right up
        Rect(0, height - height / 4, width / 4, height / 4) // down left
    };

    for (size_t i = 0; i < rois.size(); i++){
        Rect roi = rois[i] & Rect(0, 0, width, height);

        if (roi.width <= 0 || roi.height <= 0){
            CrossInfo invalid;
            invalid.detection_ok = false;
            invalid.confidence = 0.0;
            crosses.push_back(invalid);
            continue;
        }
        Mat roi_img = input_img(roi);
        CrossInfo cross = detectCross(roi_img, template_img);
        if (cross.detection_ok)
        {
            cross.location.x += roi.x;
            cross.location.y += roi.y;

            Point2f center(cross.location.x + template_img.cols / 2.0f,
                          cross.location.y + template_img.rows / 2.0f);

            bool is_top_right = (i == 0);
            bool in_correct_corner = isCrossInExpectedCorner(center, width, height, is_top_right);

            if (!in_correct_corner)
            {
                cross.detection_ok = false;
            }
        }
        crosses.push_back(cross);
    }
    return crosses;
}

// Warpping based on a rotation with 2 (or 1) crosses.
Mat wrappImageWithCrosses(const Mat& input_img, const vector<CrossInfo>& corners, const Mat& template_img){
    Mat result = input_img.clone();
    Mat rotated;
    int detected_count = 0;
    vector<Point2f> detected_points;

    for (const auto& corner : corners){
      if (corner.detection_ok){
          // add the center for the rotation around it
          Point2f center(corner.location.x + template_img.cols / 2.0f,
                          corner.location.y + template_img.rows / 2.0f);
          detected_points.push_back(center);
          detected_count++;
      }
    }

    if (detected_count < 1){
        cerr << "No crosses detected, no warpping possible" << endl;
        return input_img;

    } else if (detected_count == 1){  // 1 cross detected : rotation with the center and this cross
        Point2f detected_cross = detected_points[0];
        Point2f image_center(result.cols / 2.0f, result.rows / 2.0f);

        // Angle btw the cross and the center
        double angle_rad = atan2(image_center.y - detected_cross.y, image_center.x - detected_cross.x);
        double angle_deg = angle_rad * 180.0 / CV_PI;

        // Which cross is detected ?
        bool is_top_right = (detected_cross.x > image_center.x && detected_cross.y < image_center.y);

        // Target angle (chosen value= the one from 00000.png)
        double target_angle = is_top_right ? 125 : -65;
        double rotation_needed = target_angle - angle_deg;

        Point2f rotation_center(input_img.cols / 2.0f, input_img.rows / 2.0f);
        Mat rotation_matrix = getRotationMatrix2D(rotation_center, rotation_needed, 1.0);

        double abs_cos = abs(rotation_matrix.at<double>(0, 0));
        double abs_sin = abs(rotation_matrix.at<double>(0, 1));

        int new_width = int(result.rows * abs_sin + result.cols * abs_cos);
        int new_height = int(result.rows * abs_cos + result.cols * abs_sin);

        rotation_matrix.at<double>(0, 2) += (new_width / 2.0) - rotation_center.x;
        rotation_matrix.at<double>(1, 2) += (new_height / 2.0) - rotation_center.y;

        warpAffine(input_img, rotated, rotation_matrix, Size(new_width, new_height), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));

    }else if (detected_count == 2){ // 2 crosses detected : rotation of the sheet around the center
        Point2f p1 = detected_points[0];
        Point2f p2 = detected_points[1];

        // Which crosses are detected ?
        Point2f top_right, bottom_left;
        if (p2.x > p1.x && p2.y < p1.y){
            top_right = p2;
            bottom_left = p1;
        }else{
            top_right = p1;
            bottom_left = p2;
        }

        double angle_rad = atan2(bottom_left.y - top_right.y, bottom_left.x - top_right.x);
        double angle_deg = angle_rad * 180.0 / CV_PI;

        double target_angle = 125.0;
        double rotation_needed = angle_deg - target_angle;

        Point2f rotation_center(result.cols / 2.0f, result.rows / 2.0f);
        Mat rotation_matrix = getRotationMatrix2D(rotation_center, rotation_needed, 1.0);

        double abs_cos = abs(rotation_matrix.at<double>(0, 0));
        double abs_sin = abs(rotation_matrix.at<double>(0, 1));

        int new_width = int(result.rows * abs_sin + result.cols * abs_cos);
        int new_height = int(result.rows * abs_cos + result.cols * abs_sin);

        rotation_matrix.at<double>(0, 2) += (new_width / 2.0) - rotation_center.x;
        rotation_matrix.at<double>(1, 2) += (new_height / 2.0) - rotation_center.y;

        warpAffine(input_img, rotated, rotation_matrix, Size(new_width, new_height), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));
    }
    return rotated;
}


// For the visualisation during the test
Mat drawDetections(const Mat& input_img, const vector<CrossInfo>& corners, const Mat& template_img){
    Mat output_img = input_img.clone();

    Scalar colors[] = {
        Scalar(0, 255, 0),   // green for right-up
        Scalar(255, 0, 0)    // blue for left-down
    };

    string labels[] = {"haut-droit", "bas-gauche"};

    for (size_t i = 0; i < corners.size(); i++){
        if (corners[i].detection_ok){
            Point loc = corners[i].location;

            // rectangle
            rectangle(output_img, loc,
                     Point(loc.x + template_img.cols, loc.y + template_img.rows),
                     colors[i % 2], 3, 8, 0);

            // centre
            Point2f center(loc.x + template_img.cols / 2.0f,
                          loc.y + template_img.rows / 2.0f);
            circle(output_img, center, 8, colors[i % 2], -1);

            // label
            putText(output_img, labels[i % 2],
                   Point(center.x + 15, center.y - 10),
                   FONT_HERSHEY_SIMPLEX, 1.0, colors[i % 2], 2);

            // confidence
            string conf_text = "Conf: " + to_string((int)(corners[i].confidence * 100)) + "%";
            putText(output_img, conf_text,
                   Point(center.x + 15, center.y + 15),
                   FONT_HERSHEY_SIMPLEX, 0.6, colors[i % 2], 2);
        }
    }
    return output_img;
}


int warpping(string input_folder, string template_path, string output_folder, string wrapp_folder){
  /**
* input_folder -> path to the unwarpped images
* template_path -> path to the cross template
* output_folder -> folder where the detection of the crosses are drawn
* wrapp_folder -> folder with the warpped images
*/
    Mat template_img = imread(template_path, IMREAD_COLOR);
    if (template_img.empty())
    {
        cerr << "Erreur dans le chargement de l'image de croix: " << template_path << endl;
        return EXIT_FAILURE;
    }

    fs::create_directories(output_folder);
    fs::create_directories(wrapp_folder);

    vector<string> image_files;

    for (const auto& entry : fs::directory_iterator(input_folder)){
        image_files.push_back(entry.path().string());
    }

    int success_count = 0;
    int wrapp_count = 0;

    for (size_t i = 0; i < image_files.size(); i++){
        cout << image_files[i] << endl;
        string input_path = image_files[i];
        fs::path input_file_path(input_path);
        string filename = input_file_path.filename().string();
        retriveSymbol retrieve;
        Mat current_img = imread(input_path, IMREAD_COLOR);
        if (current_img.empty() || !retrieve.retriveSymbolFrom(current_img).second){ // c'est un dossier et non une image ou c'est une image texte
            continue;
        }

        vector<CrossInfo> corners = detectTwoCrosses(current_img, template_img);

        Mat detection_img = drawDetections(current_img, corners, template_img);
        string detection_path = output_folder + "/" + filename;
        if (imwrite(detection_path, detection_img)){
            success_count++;
        }

        Mat wrapped = wrappImageWithCrosses(current_img, corners, template_img);
        string wrapp_path = wrapp_folder + "/" + filename;
        if (imwrite(wrapp_path, wrapped)){
            wrapp_count++;
        }
    }
    cout << "Saved detections : " << success_count << endl;
    cout << "wrapped images : " << wrapp_count << endl;

    return EXIT_SUCCESS;
}

