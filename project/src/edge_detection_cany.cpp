//
// Parcourir les images du dossier 'extract_bd' et y appliquer l'edge detection de cany pour stocker les résultats dans 'extract_bd/edges'
//
// Léna

#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

#include "edge_detection_cany.hpp"

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

int main() {
    string inputs = "extract_bd"; // dossier avec les images
    string outputs = "extract_bd/edges"; // dossier pour stocker les contours

    
    // Parcourir les image
    for(const auto &input : fs::directory_iterator(inputs)) {
        if(fs::is_regular_file(input)) {
            Mat img = imread(input.path().string());
            if (img.empty()) {
                cerr << "Error opening file " << input.path().string() << endl;
                continue;
            }
            
            Mat img_gray, img_canny, img_dilate;
            cvtColor(img, img_gray, COLOR_BGR2GRAY);
            Canny(img_gray, img_canny, 25, 75);
            Mat se1 = getStructuringElement(MORPH_RECT, Size(5, 5));
            dilate(img_canny, img_dilate, se1);
            Mat result = img_dilate.clone();
            
            string output_path = outputs + "/" + input.path().filename().string();
            if (imwrite(output_path, result)) {
                cout << "Sauvegardé: " << output_path << endl;
            } else {
                cerr << "Erreur lors de la sauvegarde: " << output_path << endl;
            }
        }
    }
    
    cout << "Traitement terminé!" << endl;
    return 0;
}