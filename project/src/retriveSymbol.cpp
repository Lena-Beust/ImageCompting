//
// Created by anais on 11/12/2025.
//

#include "retriveSymbol.h"

#include <cstdlib>

//  ---   Function to extract the squares   ---   //
void retriveSymbol::detect_squares(Mat& dial_img, Mat& in_img)
{
    //  ---  Initialisation values  ---  //
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<pair<Mat, Rect>> squaresWithPos;
    vector<Point> poly;
    int tol = 10;
    int margin;
    float ratio;
    double area;
    double peri;
    Rect r;
    Mat squareROI ;

    // ---  Find box with contours detection ---  //

    findContours(dial_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i)
    {
        // -- Verify the shape is a square -- //

        area = contourArea(contours[i]);
        if (area < 50000) continue;   // verify the square is big enough to be the box
        if (area > 1000000)   // Verify the square isn't to big and as such a text area
        {
            valid = false;  // if it's a text area the image isn't valid and should not be treated
            squares.push_back(in_img);
            return;
        }

        peri = arcLength(contours[i], true);
        approxPolyDP(contours[i], poly, 0.02 * peri, true);  // simplify the outline

        if (poly.size() != 4) // Verify the shape have 4 vertices
            continue;

        r = boundingRect(poly);

        // Vérification du rapport largeur/hauteur
        ratio = (float)r.width / (float)r.height;

        // square tolerance, the width/height ratio shouldn't be too big
        if (ratio < 0.80 || ratio > 1.20)
            continue;

        // -- Cut the picture so only what's in the box remain -- //

        margin = max(10, r.width / 100);

        r.x += margin;
        r.y += margin;
        r.width  -= 2 * margin;
        r.height -= 2 * margin;

        // Safety measures to prevent overruns
        r &= Rect(0, 0, in_img.cols, in_img.rows);

        // Extract the area corresponding to the square from source_img
        Mat squareROI = in_img(r).clone();  // clone to copy
        squaresWithPos.push_back({squareROI, r});
    }

    //  ---  sorting, first by y then by x  ---  //
    sort(squaresWithPos.begin(), squaresWithPos.end(),
         [tol](const pair<Mat, Rect>& a, const pair<Mat, Rect>& b) {
             if (abs(a.second.y - b.second.y) > tol)
                 return a.second.y < b.second.y;  // line
             return a.second.x < b.second.x;      // column
         });
    squares.clear();
    for (auto& p : squaresWithPos)
        squares.push_back(p.first);
}

//  ---  Main function  --- ///

pair<vector<Mat>,bool> retriveSymbol::retriveSymbolFrom(Mat& img)
{
    //  ---  Initialisation values  ---  //
    Mat hsv, mask_blue, cleaned;
    Mat img_gray, img_blur, img_canny, img_dilate, img_erode,result;
    Mat se1, se2, maskBlueColor;

    // --- Preprocessing image  --- //

    //  --  extraction of the blues  ---  //
    cvtColor(img, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(80, 50, 50), Scalar(120, 255, 255), mask_blue);
    img.copyTo(cleaned);
    cleaned.setTo(255, mask_blue); // remove the bleu by making them white

    // --  Clearing the boxes --  //
    cvtColor(cleaned,img_gray,COLOR_BGR2GRAY);    // convert to black and white
    GaussianBlur(img_gray, img_blur, Size(3, 3), 3, 0); // blurring image using gaussian fliter.
    Canny(img_blur, img_canny, 25, 75);   // edge detection using canny algo

    se1 = getStructuringElement(MORPH_RECT, Size(5, 5)); // setting the structuring element
    dilate(img_canny, img_dilate, se1);     // dialating image

    se2 = getStructuringElement(MORPH_RECT, Size(4,4)); // setting the structuring element
    morphologyEx(img_dilate, img_dilate, MORPH_CLOSE, se2);  // morphological closing operation


    //  --  Using the detect_squares to extract the squares  --  //
    detect_squares(img_dilate, img);

    return {squares, valid};
}
