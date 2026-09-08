#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include<iostream>
#include "zoom.hpp"

using namespace cv;
using namespace std;

// Variables globales
Mat img_final;       // image finale (Laplacian)
int windowWidth = 400;
int windowHeight = 600;

double zoom = 1.0;   // facteur de zoom
double minZoom = 0.1; // zoom minimum
int zoom_slider = 100;

Point panOffset(0, 0);   // position de la portion affichée
bool isDragging = false;
Point dragStart;

// Fonction pour afficher l'image avec zoom et pan
/**
void showImage() {
    // Taille de la portion à afficher
    int cropW = static_cast<int>(windowWidth / zoom);
    int cropH = static_cast<int>(windowHeight / zoom);

    // Limiter la taille à la taille de l'image
    cropW = min(cropW, img_final.cols);
    cropH = min(cropH, img_final.rows);

    // Calculer la portion avec panOffset
    int x = panOffset.x;
    int y = panOffset.y;

    // S'assurer qu'on ne sort pas de l'image
    x = max(0, min(x, img_final.cols - cropW));
    y = max(0, min(y, img_final.rows - cropH));

    Mat roi = img_final(Rect(x, y, cropW, cropH));

    // Redimensionner pour la fenêtre
    Mat displayed;
    resize(roi, displayed, Size(windowWidth, windowHeight));

    imshow("Affichage image", displayed);
}
*/
void showImage() {
    // Calculer la taille affichée avec le zoom
    int displayW = static_cast<int>(img_final.cols * zoom);
    int displayH = static_cast<int>(img_final.rows * zoom);

    // Si l'image zoomée est plus petite que la fenêtre, on la centre
    int offsetX = 0, offsetY = 0;
    if (displayW < windowWidth) {
        offsetX = (windowWidth - displayW) / 2;
    }
    if (displayH < windowHeight) {
        offsetY = (windowHeight - displayH) / 2;
    }

    // Créer une image noire de la taille de la fenêtre
    Mat displayed = Mat::zeros(windowHeight, windowWidth, img_final.type());

    // Calculer la portion visible de l'image
    int viewW = min(windowWidth, displayW);
    int viewH = min(windowHeight, displayH);

    // Limiter panOffset pour ne pas sortir de l'image zoomée
    int maxPanX = max(0, displayW - windowWidth);
    int maxPanY = max(0, displayH - windowHeight);
    panOffset.x = max(0, min(panOffset.x, maxPanX));
    panOffset.y = max(0, min(panOffset.y, maxPanY));

    // Redimensionner l'image avec le zoom
    Mat resized;
    resize(img_final, resized, Size(displayW, displayH), 0, 0, INTER_LINEAR);

    // Extraire la portion visible
    int srcX = panOffset.x;
    int srcY = panOffset.y;
    int srcW = min(viewW, resized.cols - srcX);
    int srcH = min(viewH, resized.rows - srcY);

    if (srcW > 0 && srcH > 0) {
        Mat roi = resized(Rect(srcX, srcY, srcW, srcH));

        // Copier dans l'image affichée
        int dstX = (displayW < windowWidth) ? offsetX : 0;
        int dstY = (displayH < windowHeight) ? offsetY : 0;
        roi.copyTo(displayed(Rect(dstX, dstY, srcW, srcH)));
    }

    imshow("Affichage image", displayed);
}

// Callback souris pour pan
void onMouse(int event, int x, int y, int, void*) {
    if (event == EVENT_LBUTTONDOWN) {
        isDragging = true;
        dragStart = Point(x, y);
    } else if (event == EVENT_MOUSEMOVE && isDragging) {
        Point delta = dragStart - Point(x, y);
        panOffset += Point(static_cast<int>(delta.x / zoom), static_cast<int>(delta.y / zoom));
        dragStart = Point(x, y);
        showImage();
    } else if (event == EVENT_LBUTTONUP) {
        isDragging = false;
    }
}

// Callback trackbar pour zoom
void onZoomSlider(int, void*) {
    zoom = zoom_slider / 100.0;
    if (zoom < minZoom) zoom = minZoom;
    showImage();
}

void explore_images() {
    string path = "../../NicIcon/all-scans/"; // Path to adapt for your own folder

    for (int i =0; i <1; i++) { // Number of "folder" to work with
        for (int j = 0 ; j < 23; j++) {

            string folder, file ;
            if (i < 10) {folder = "0" + to_string(i) ;}
            else { folder = to_string(i) ; }

            if (j < 10) {file = "0" + to_string(j) ;}
            else { file = to_string(j) ; }

            string imName = path + "0" + folder + file + ".png";

            Mat im = imread(imName);
            if(im.data == nullptr){
                cerr << "Image not found: "<< imName << endl;
                waitKey(0);
                //system("pause");
                exit(EXIT_FAILURE);
            }
            // Define new width and height
            // Arbitrary values to fit our images
            int new_width = 496;
            int new_height = 701.6;
            Mat resized_image;
            resize(im, resized_image, Size(new_width, new_height));

            imshow(imName, resized_image);
        }
    }
    waitKey(0); // Quit the program whenever a key is pressed
}

int image_reader(string path) {
    // ------- ZONE INITIALISATION DE L'IMAGE ------- //
    Mat img = imread(path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "Erreur : impossible de charger l'image !" << endl;
        return -1;
    }

    // ------- ZONE OPERATION ------- //
    Mat laplacian;
    Laplacian(img, laplacian, CV_64F);
    convertScaleAbs(laplacian, img_final);

    // mettez la version finale de l'image, celle que vous voulez afficher dans img_final

    // ------- ZONE AFFICHAGE ------- //
    // Calcul du zoom minimum pour afficher toute l'image dans la fenêtre
    double zoomX = static_cast<double>(windowWidth) / img_final.cols;
    double zoomY = static_cast<double>(windowHeight) / img_final.rows;
    minZoom = min(zoomX, zoomY);
    zoom = minZoom;
    zoom_slider = static_cast<int>(zoom * 100);

    panOffset = Point(0, 0);

    // Création de la fenêtre et trackbar
    namedWindow("Affichage image", WINDOW_NORMAL);
    resizeWindow("Affichage image", windowWidth, windowHeight);
    createTrackbar("Zoom %", "Affichage image", &zoom_slider, 200, onZoomSlider);
    setMouseCallback("Affichage image", onMouse);

    // Affichage initial
    showImage();

    cout << "Zoom avec la trackbar et déplacez l'image avec clic/glisser." << endl;

    // Boucle pour que la fenêtre reste responsive
    while (true) {
        int key = waitKey(30);
        if (key >= 0) break;
    }

    return 0;
}


