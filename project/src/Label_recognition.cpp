//
// Created by vnithian on 28/12/25.
//

#include "Label_recognition.h"
/*Test code on the main :
#include "Label_recognition.h"
int main() {
    //chemin à modifier en fonction de où le make est build
    //ici make build dans le dossier build
    //si chemin à modifier, modifier aussi dans la fonction traitement (ligne 24, chemin pour les icones)
    Mat img_a_traiter=imread("../extract_bd/00601.png");
    Label_recognition reL=Label_recognition(img_a_traiter);
    reL.association();
    reL.afficheDescription();
    return 0;
}
 */
map<float,string> Label_recognition::traitement(string type) {
    map<float,string> icones_founded;
    double score_threshold = 0.6;
    //base d'entrainement : double score_threshold = 0.67;
    Mat results,debug;
    debug=img.clone();
    std::list<std::string>::const_iterator i;
    std::list<std::string>::const_iterator fin;
    if(type=="size") {
        i=icones_size.begin();
        fin=icones_size.end();
    }
    else {
        i=icones_name.begin();
        fin=icones_name.end();
    }
    for(i; i !=fin; ++i) {
        Mat imgGray,tplGray,imgBin,tplBin;

        string name=i->c_str();
        Mat icone=imread("../icones/"+name+".png");
        if (img.empty()||icone.empty()) {
            cout << "Erreur : impossible de charger l'image !" << endl;
            return icones_founded;
        }
        // template matching méthode
        int method = TM_CCOEFF_NORMED;
        // transforme les images en binaire
        cvtColor(img, imgGray, COLOR_BGR2GRAY);
        cvtColor(icone, tplGray, COLOR_BGR2GRAY);
        //met au même niveau de noir et blanc grâce aux histos
        Ptr<CLAHE> clahe = createCLAHE(2.5, Size(8,8));
        clahe->apply(imgGray, imgGray);
        clahe->apply(tplGray, tplGray);
        normalize(imgGray, imgGray, 0, 255, NORM_MINMAX);
        normalize(tplGray, tplGray, 0, 255, NORM_MINMAX);
        if (mean(imgGray)[0] > mean(tplGray)[0]) {
            bitwise_not(imgGray, imgGray);
            bitwise_not(tplGray, tplGray);
        }
        //léger flou
        GaussianBlur(imgGray, imgGray, Size(3,3), 0);
        GaussianBlur(tplGray, tplGray, Size(3,3), 0);
        results.create(imgGray.rows - tplGray.rows + 1, imgGray.cols - tplGray.cols + 1, CV_32FC1);
        //template matching
        matchTemplate(imgGray, tplGray, results, method);
        while(true) {
            double minVal, maxVal;
            Point minLoc, maxLoc;
            minMaxLoc(results, &minVal, &maxVal, &minLoc, &maxLoc);

            if(maxVal < score_threshold) break; // évite les faux positifs
            if(maxLoc.x < x_min||maxLoc.x > x_max || maxLoc.y < y_min || maxLoc.y > y_max) {
                results.at<float>(maxLoc.y, maxLoc.x) = 0;
                continue;
            }

            // créer le rectangle autour de la détection
            Rect rect(maxLoc.x, maxLoc.y, icone.cols, icone.rows);
            float center_y = rect.y + rect.height / 2;
            icones_founded.insert({center_y, name});

            rectangle(debug, rect, Scalar(0, 255, 0), 2);

            // Masquer la zone détectée pour éviter doublons
            int x0 = max(0, maxLoc.x - icone.cols/2);
            int y0 = max(0, maxLoc.y - icone.rows/2);
            int x1 = min(results.cols, maxLoc.x + icone.cols/2);
            int y1 = min(results.rows, maxLoc.y + icone.rows/2);
            results(Range(y0, y1), Range(x0, x1)) = Scalar(0);
        }
        }

/*
    namedWindow("correlation", WINDOW_NORMAL);
    imshow("correlation", debug);
    //showImage(img);
    waitKey(0);
*/
    return icones_founded;
}

//associe chaque taille à son icone (prend en compte le cas où sur une page, certaines icones ont des tailles et d'autres non)
void Label_recognition::association() {
    icones_founded_name=traitement("name");
    map<float,string>icones_founded_size=traitement("size");
    if(icones_founded_size.size()>7) {
        return;
    }
    map<float, string>::iterator it;
    int compteur=0;
    for (auto i :icones_founded_size) {
        for (auto it = icones_founded_name.begin(); it != icones_founded_name.end(); ++it){
            auto pro=next(it);
            if(pro==icones_founded_name.end()&&i.second!="") {
                icones_taille[compteur]=i.second;
            }
            //si la coordonnée y de la taille est supérieure à celle de la coordonnée y de l'icone actuel
            // et inférieure à celle de la coordonnée y du prochain icone dans la map
            // alors le label de la taille appartient à celle de l'icone au-dessus
            //y=0 pour le coin gauche supérieur (y croît en descendant sur l'image
            else if (it->first<=i.first&&i.first<=pro->first) {
                //it->second+=" "+i.second;
                icones_taille[compteur]=i.second;
                break;
            }
        }
        compteur++;
    }
}

//print on the terminal the description
void Label_recognition::afficheDescription() {
    int compteur=0;
    for(map<float,string>::iterator it=icones_founded_name.begin();it!=icones_founded_name.end();it++) {
        cout<<"label "<<it->second<<"\n";
        if(icones_taille[compteur]!="") {
            cout<<"size "<<icones_taille[compteur]<<"\n";
        }
        compteur++;
        cout<<"\n";
    }
}
//get taille
vector<string> Label_recognition::getSize() {
    return icones_taille;
}
//get label name
vector<string> Label_recognition::getLabel() {
    vector<string> labels;
    for(map<float,string>::iterator it=icones_founded_name.begin();it!=icones_founded_name.end();it++) {
        labels.push_back(it->second);
    }
    return labels;
}