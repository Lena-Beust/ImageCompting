#include <fstream>

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include<iostream>

using namespace cv;
using namespace std;

#include "binary_code.hpp"
#include "zoom.hpp"
#include "edge_detection_cany.hpp"
#include "warpping.hpp"
#include "Label_recognition.h"
#include <filesystem>
#include "retriveSymbol.h"
#include "affichage.h"

void delete_dir_content(const fs::path& dir_path) {
    for (auto& path: fs::directory_iterator(dir_path)) {
        fs::remove_all(path);
    }
}
int main (void) {

    string input_folder = "../extract_bd";
    string output_folder = "../extract_bd/cross_detection";
    string wrapp_folder = "../extract_bd/wrapped";
    string template_path = "../extract_bd/cross_template/Cross_template.png";
    string writer_page="../extract_bd/writer_page_ID/";
    delete_dir_content(output_folder);
    delete_dir_content(wrapp_folder);
    int succes=warpping(input_folder, template_path, output_folder, wrapp_folder);
    string output="../resultat/";
    fs::create_directories(output);
    delete_dir_content(output);
    if(succes==EXIT_SUCCESS) {
        binarize_dilation_all(wrapp_folder); //ne pas oublier de changer le chemin dans binary_code avec extract_bd
        fs::create_directories(writer_page);
        vector<string>forms=detect_outlines_all(writer_page);
        vector<fs::path> wrapped_files;
        for (const auto& e : fs::directory_iterator(wrapp_folder)) {
            if (e.is_regular_file() && e.path().extension() == ".png") {
                wrapped_files.push_back(e.path());
            }
        }
        //tri par le nom du fichier
        sort(wrapped_files.begin(), wrapped_files.end());

        for (size_t c = 0; c < min(wrapped_files.size(), forms.size()); c++){
                string form=forms[c];
                string scripter=form.substr(0,3);
                string page=form.substr(3,2);
                Mat img_a_traiter=imread(wrapped_files[c].string());
                if (img_a_traiter.empty()) {
                cerr << "Erreur : impossible de charger l'image " << wrapped_files[c].string() << endl;
                continue;  // passer à l'image suivante
                }
                Label_recognition reL=Label_recognition(img_a_traiter);
                reL.association();
                vector<string> labels=reL.getLabel();
                vector<string>sizes=reL.getSize();
                retriveSymbol retrieve;
                vector<Mat> images=retrieve.retriveSymbolFrom(img_a_traiter).first;
                string description;
                int index=0;
                if (labels.size()!=7||sizes.size()>7) {
                    cerr << "Doublons détectés des icônes/tailles dans le fichier : "<<wrapped_files[c].string() << endl;
                    continue;
                }
                for (int row=1;row<=7;row++) {
                    for (int column=2;column<=6;column++) {
                        if (index >= images.size()) {
                            cout << wrapped_files[c].string() << " -> images.size() = " << images.size() << endl;
                            break;
                        }
                        int label_index = row - 1;
                        ofstream outputFile;
                        description="#projet TIV 2025-2026 groupe A BEUST_GRANGER_MERCIER_NITHIANANTHAM \n";
                        description+="label "+labels[label_index]+" \n";
                        description+="form "+form+"\n";
                        description+="scripter "+scripter+"\n";
                        description+="page "+page+"\n";
                        description+="row "+to_string(row)+"\n";
                        description+="column "+to_string(column)+"\n";
                        if(sizes[label_index]!="") {
                            description+="size "+sizes[label_index]+"\n";
                        }
                        string file_name=labels[label_index]+"_"+scripter+"_"+page+"_"+to_string(row)+"_"+to_string(column);
                        outputFile.open (output+file_name+".txt", std::ios_base::app);
                        outputFile << description;
                        outputFile.close();
                        imwrite(output+file_name+".png", images[index]);
                        description="";
                        index++;
                    }
                }
        }
    }


/*
    //Mat img_a_traiter=imread("../../echantillon_base_finale/wrapped/s01_0001.png");
    Mat img_a_traiter=imread("../extract_bd/wrapped/00205.png");
    Label_recognition reL=Label_recognition(img_a_traiter);
    vector<string> labels=reL.getLabel();
    vector<string>sizes=reL.getSize();
    if (labels.size()!=7||sizes.size()>7) {
        cout << "Doublons détectés des icônes/tailles dans le fichier " << endl;
        return 0;
    }
    reL.association();
    reL.afficheDescription();
    return 0;
*/
/*
    string folder="extract_bd";
    string folder1 = "../"+folder+"/wrapped";
    string writer_page="../"+folder+"/writer_page_ID/";
    binarize_dilation_all(folder1); //ne pas oublier de changer le chemin dans binary_code avec extract_bd
    vector<string>forms=detect_outlines_all(writer_page);
    for (string s : forms) {
        cout<<s<<endl;
    }
*/
/*
     //afficher chaque sous-image
    Mat img_a_traiter = imread("../w030-scans/03022.png");
    if (img_a_traiter.empty()) {
        cerr << "Erreur : impossible de charger l'image !" << endl;
        return -1;
    }
    retriveSymbol retive;
    if(!retive.retriveSymbolFrom(img_a_traiter).second) {
        return 0;
    }
    vector<Mat> squares = retive.retriveSymbolFrom(img_a_traiter).first;

    for (size_t i = 0; i < squares.size(); ++i) {
        string winName = "SYMBOLE " + to_string(i+1);
        imshow(winName, squares[i]);   // square[i] = vrai contenu couleur
        waitKey(0);                    // attend une touche pour passer à l'image suivante
        destroyWindow(winName);        // ferme la fenêtre avant la suivante
    }
    */

    return 0;
}
