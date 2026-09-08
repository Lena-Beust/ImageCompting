using namespace cv;
using namespace std;

#ifndef PROJECT_BINARY_CODE_H
#define PROJECT_BINARY_CODE_H


int sobel_detection(string path);

void binarize_dilation(string path, string name="");
void binarize_dilation_all(string folder);

string detect_outlines(string path);
vector<string> detect_outlines_all(string folder);

void detect_rectangle(string path);
void detect_rectangle_all(string folder);



#endif //PROJECT_BINARY_CODE_H