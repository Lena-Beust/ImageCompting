using namespace cv;
using namespace std;

#ifndef PROJECT_ZOOM_H
#define PROJECT_ZOOM_H

void showImage(); // Show image with zoom and pan

void onMouse(int event, int x, int y, int, void*); // Callback mouse for pan

void onZoomSlider(int, void*); // Callback trackbar for zoom

void explore_images(); // Open a window for each image

int image_reader(string path);

#endif //PROJECT_ZOOM_H