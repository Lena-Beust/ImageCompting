//
// Created by anais on 11/12/2025.
//

#include "../include/affichage.h"


// This Class is only to ease testing and isn't use in the final project

// Constructeur
// .cpp
affichage::affichage()
    : windowWidth(600), windowHeight(900),
      zoom(1.0), minZoom(1.0),
      panOffset(0,0), isDragging(false),
      zoom_slider(100) {}

// Méthode interne
void affichage::showImageInternal() {
    if (img_final.empty()) return;

    int cropW = static_cast<int>(windowWidth / zoom);
    int cropH = static_cast<int>(windowHeight / zoom);

    cropW = min(cropW, img_final.cols);
    cropH = min(cropH, img_final.rows);

    int x = max(0, min(panOffset.x, img_final.cols - cropW));
    int y = max(0, min(panOffset.y, img_final.rows - cropH));

    Mat roi = img_final(Rect(x, y, cropW, cropH));
    Mat displayed;
    resize(roi, displayed, Size(windowWidth, windowHeight));
    imshow("Affichage image", displayed);
}

// Méthodes statiques
void affichage::onMouseStatic(int event, int x, int y, int flags, void* userdata) {
    affichage* self = static_cast<affichage*>(userdata);
    if (self) self->onMouse(event, x, y, flags);
}

void affichage::onZoomSliderStatic(int value, void* userdata) {
    affichage* self = static_cast<affichage*>(userdata);
    if (self) {
        self->zoom_slider = value;
        self->onZoomSlider();
    }
}

// Méthodes membres réelles
void affichage::onMouse(int event, int x, int y, int flags) {
    if (event == EVENT_LBUTTONDOWN) {
        isDragging = true;
        dragStart = Point(x, y);
    } else if (event == EVENT_MOUSEMOVE && isDragging) {
        Point delta = dragStart - Point(x, y);
        panOffset += Point(static_cast<int>(delta.x / zoom), static_cast<int>(delta.y / zoom));
        dragStart = Point(x, y);
        showImageInternal();
    } else if (event == EVENT_LBUTTONUP) {
        isDragging = false;
    }
}

void affichage::onZoomSlider() {
    zoom = zoom_slider / 100.0;
    if (zoom < minZoom) zoom = minZoom;
    showImageInternal();
}

// Public
void affichage::showImage() { showImageInternal(); }

int affichage::afficher(Mat img) {
    img_final = img.clone();
    double zoomX = static_cast<double>(windowWidth) / img_final.cols;
    double zoomY = static_cast<double>(windowHeight) / img_final.rows;
    minZoom = min(zoomX, zoomY);
    zoom = minZoom;
    zoom_slider = static_cast<int>(zoom * 100);
    panOffset = Point(0,0);

    namedWindow("Affichage image", WINDOW_NORMAL);
    resizeWindow("Affichage image", windowWidth, windowHeight);
    createTrackbar("Zoom %", "Affichage image", &zoom_slider, 200, onZoomSliderStatic, this);
    setMouseCallback("Affichage image", onMouseStatic, this);

    showImageInternal();

    while (true) {
        int key = waitKey(30);
        if (key >= 0) break;
    }

    return 0;
}
