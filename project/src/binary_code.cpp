#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include<iostream>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

#include "binary_code.hpp"
#include "zoom.hpp"



// Binarize then dilate the image given, and keep only its binary code
void binarize_dilation(string path, string name){
    Mat src, resized_src, binary, element, dilation_dst;
    src = cv::imread(path, cv::IMREAD_GRAYSCALE);
    resize(src, resized_src, Size(496, 702));

    element = getStructuringElement( MORPH_RECT,
                         Size( 2, 2 ),
                         Point( 0, 0 ) );

	threshold(resized_src, binary, 200, 255, THRESH_BINARY);
    dilate(binary, dilation_dst, element );

    string output_path = "../extract_bd/writer_page_ID/" + name;
    if (imwrite(output_path, dilation_dst)) {
        cout << "Sauvegardé: " << output_path << endl;
    } else {
        cerr << "Erreur lors de la sauvegarde: " << output_path << endl;
    }
}

// Loops on all images of the folder, to dilate them all
void binarize_dilation_all(string folder){
    // Parcourir les images
    for(const auto &input : fs::directory_iterator(folder)) {
        if(fs::is_regular_file(input)) {
            Mat img = imread(input.path().string());
            if (img.empty()) {
                cerr << "Error opening file " << input.path().string() << endl;
                continue;
            }
            binarize_dilation(input.path().string(), input.path().filename().string());
        }
    }
}

// Detects outlines on a single image and saves the result
string detect_outlines(string path) {
    Mat src_gray, canny_output;
    int thresh = 100;
    RNG rng(12345);

    Mat img = imread(path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cerr << "Error opening file " << path << endl;
        return "";
    }

    // Detect edges
    Canny(img, canny_output, thresh, thresh*2);

    // Find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    Rect best;
    double bestScore = 0;

    int bestI = -1;

    for (int i = 0; i < contours.size(); i++) {
        Rect r = boundingRect(contours[i]);
        double aspect = (double)r.width / r.height;
        double area = r.area();

        if (aspect > 5.0 && area > bestScore) {
            bestScore = area;
            best = r;
            bestI = i;
        }
    }

    if (bestI < 0) {
        cerr << "No suitable contour found" << endl;
        return "";
    }

    // Crop the image
    Mat cropped = img(best).clone();

    // Projection on a single axis (1D)
    Mat projection, smooth;


    // This will get the median (less sensible to noise)

    vector<uchar> col;
    Mat proj(1, cropped.cols, CV_8U);

    for (int x = 0; x < cropped.cols; x++) {
        col.clear();
        for (int y = 0; y < cropped.rows; y++)
            col.push_back(cropped.at<uchar>(y, x));

        nth_element(col.begin(),
                     col.begin() + col.size()/2,
                     col.end());
        proj.at<uchar>(x) = col[col.size()/2];
    }


    // Binarize the signal

    vector<int> binarized_1D;
    binarized_1D.reserve(proj.cols);
    for (int i = 0; i < proj.cols; i++) {
        int v = proj.at<uchar>(i);
        binarized_1D.push_back(v > 130 ? 1 : 0);
    }


    // Transform the signal into a binary code
    std::vector<int> runs;

    int current = binarized_1D[4];
    int length = 5;

    // Makes it start after the beginning, to be sure to only capt the black pixels
    for (int i = 5; i < binarized_1D.size(); i++) {
        if (binarized_1D[i] == current) {
            length++;
        } else {
            //cout << length << ' ';
            runs.push_back(length);
            current = binarized_1D[i];
            length = 1;
        }
    }
    runs.push_back(length);


    // -------------------NORMALIZE THE VECTOR-------------------------

    // Find the unit for this specific image
    int unit = 50;
    for (int i = 0 ; i < runs.size(); i++) {
        if (runs[i] > 4 && runs[i] < unit) {
            unit = runs[i] + 1;
        }
    }

    vector<int> unitRuns;
    for (int i = 0 ; i < runs.size(); i++) {
        if (runs[i] < unit-1) { // If the value is a separator, then push 0
            unitRuns.push_back(0);
            continue;
        }
        int u = std::round((double)runs[i] / unit); // else push the value normalized
        unitRuns.push_back(std::max(1, u));
    }

    // Transforms the vector into a binarized_1D code

    vector<int> binary_code;
    int color = 0; // first run is black

    for (int u : unitRuns) {
        if (u == 0) {
            color = 1 - color;
            continue;
        }
        for (int i = 0; i < u; i++)
            binary_code.push_back(color);
        color = 1 - color;
    }


    // --------------------BINARY CODE HERE----------------------------
    // -------------------TRANSFORM THE BINARY CODE INTO A DIGIT CODE-----------------------

    // Remove firsts 0
    auto it = std::find(binary_code.begin(), binary_code.end(), 1);
    binary_code.erase(binary_code.begin(), it);

    // Compute the code

    int value = 0;

    for (int bit : binary_code) {
        value = (value << 1) | bit;
    }


    // -------------------------TEST-----------------------------
    // use " ./Projet_OpenCV_CMake | wc -l" in terminal to get the number of image tested, and
    // " ./Projet_OpenCV_CMake | grep "0"| wc -l" to see how many errors on those
    /*
    ostringstream oss;
    oss << std::setw(5) << std::setfill('0') << value;
    string padded = oss.str() + ".png";
    if(name!="") {
        cout << (padded == name) << endl;
    }
    */
    ostringstream oss;
    oss << std::setw(5) << std::setfill('0') << value;
    string padded = oss.str();
    return padded;

}

// Loops through all images in a folder and detects outlines
vector<string> detect_outlines_all(string folder) {
    vector<string> tab;
    for (const auto &input : fs::directory_iterator(folder)) {
        if (fs::is_regular_file(input)) {
            tab.push_back(detect_outlines(input.path().string()));
        }
    }
    return tab;
}
