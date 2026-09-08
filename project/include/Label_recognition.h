//
// Created by vnithian on 28/12/25.
//

#ifndef LABEL_RECOGNITION_H
#define LABEL_RECOGNITION_H
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Label_recognition {
private:
	const int y_min=700;
	const int y_max=3200;
	const int x_min=100;
	const int x_max=400;
	Mat img;
	map<float,string>icones_founded_name;
	vector<string> icones_taille;
	const list<string> icones_name={"Accident","Bomb","Car","Casualty","Electricity","Fire","Fire brigade","Flood","Gas","Injury","Paramedics","Person","Police","Road block"};
	const list<string> icones_size={"large","medium","small"};

public:
	Label_recognition(Mat img_a_traiter):img(img_a_traiter),icones_taille(7) {};
	map<float,string> traitement(string type);
	void association();
	void afficheDescription();
	vector<string> getSize();
	vector<string> getLabel();
};
#endif //LABEL_RECOGNITION_H
