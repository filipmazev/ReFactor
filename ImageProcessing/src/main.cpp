#include "hazeremoval.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const string BASE_PATH = "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/";

Mat loadImage(const std::string& path) {
    Mat image = imread(path, IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error loading image!" << std::endl;
        exit(-1);
    }
    return image;
}

int main(int argc, char **args) {
	const string IMAGE_PATH = BASE_PATH + "forest1.jpg";
	
	Mat in_img = loadImage(IMAGE_PATH);
	Mat out_img(in_img.rows, in_img.cols, CV_8UC3);
	unsigned char * indata = in_img.data;
	unsigned char * outdata = out_img.data;
	unsigned char * fogdata = out_img.data;

	CHazeRemoval hr;
	cout << hr.InitProc(in_img.cols, in_img.rows, in_img.channels()) << endl;
	cout << hr.Process(indata, outdata, fogdata, in_img.cols, in_img.rows, in_img.channels()) << endl;
	
	imshow("Fog Image", out_img);
	imwrite(BASE_PATH + "forest1_fog.jpg", out_img);

	waitKey(0);
	return 0;
}