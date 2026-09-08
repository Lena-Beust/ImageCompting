//
// Created by anais on 11/12/2025.
//

#ifndef PROJET_TIV_AFFICHAGE_H
#define PROJET_TIV_AFFICHAGE_H
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// .h
class affichage {
private:
    Mat img_final;
    int windowWidth, windowHeight;
    double zoom, minZoom;
    Point panOffset;
    bool isDragging;
    Point dragStart;
    int zoom_slider;

    void showImageInternal();

    // Méthodes statiques pour OpenCV
    static void onMouseStatic(int event, int x, int y, int flags, void* userdata);
    static void onZoomSliderStatic(int value, void* userdata);

    void onMouse(int event, int x, int y, int flags);
    void onZoomSlider();

public:
    affichage();
    void showImage();
    int afficher(Mat img);
};


#endif