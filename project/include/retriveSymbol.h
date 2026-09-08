//
// Created by anais on 11/12/2025.
//

#ifndef PROJET_TIV_RETRIVESYMBOL_H
#define PROJET_TIV_RETRIVESYMBOL_H
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <utility>
#include <string>


using namespace cv;
using namespace std;

class retriveSymbol
{
    private:
    vector<Mat> squares;
    bool valid = true;
    void detect_squares(Mat& dial_img, Mat& in_img);
    public:
    pair<vector<Mat>,bool> retriveSymbolFrom(Mat& img);
};


#endif //PROJET_TIV_RETRIVESYMBOL_H