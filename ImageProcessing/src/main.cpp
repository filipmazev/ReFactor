#include "hazeremoval.h"
#include "other/arr.h"
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <filesystem>

using namespace cv;
using std::cout;
using std::endl;

namespace fs = std::filesystem;

const std::string BASE_PATH = "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/";
const std::string OUTPUT_FOLDER_DESTINATION = "output/";
const std::string IMAGE_FOLDER_PREFIX = "ouput_";
const std::string FILE_TYPE_SPLITTER = ".";
const std::string FOLDER_SPLITTER = "/";
const std::string FOLDER_PATH_SPLITTER = "/\\";
const std::string FILE_NAME_SPACE_DIVIDER = "_";

Mat loadImage(const std::string& path) {
    Mat image = imread(path, IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error loading image!" << std::endl;
        exit(-1);
    }
    return image;
}

std::vector<std::string> getImagesInFolder(const std::string& folderPath) {
	std::vector<std::string> imagePaths;
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file()) {
			std::string path = entry.path().string();
			if (path.find(".jpg") != std::string::npos || path.find(".png") != std::string::npos) {
				imagePaths.push_back(path);
			}
		}
	}
	return imagePaths;
}

std::oth::arr<int> calculateHistograms(const Mat& image) {
	std::oth::arr<int> histograms;
	histograms.resize(256);
	histograms.fill(0);

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			histograms[image.at<uchar>(i, j)]++;
		}
	}
	return histograms;
}

void processHistogram(const Mat& image, std::string folderPath, std::string IMAGE_NAME, std::string IMAGE_TYPE)
{
	std::vector<Mat> bgr_planes;
	split(image, bgr_planes);

	std::oth::arr<int> hist_b = calculateHistograms(bgr_planes[0]);
	std::oth::arr<int> hist_g = calculateHistograms(bgr_planes[1]);
	std::oth::arr<int> hist_r = calculateHistograms(bgr_planes[2]);

	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double) hist_w / 256);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	for (int i = 1; i < 256; i++) {
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist_b[i - 1])),
			 Point(bin_w * i, hist_h - cvRound(hist_b[i])),
			 Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist_g[i - 1])),
			 Point(bin_w * i, hist_h - cvRound(hist_g[i])),
			 Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist_r[i - 1])),
			 Point(bin_w * i, hist_h - cvRound(hist_r[i])),
			 Scalar(0, 0, 255), 2, 8, 0);
	}

	imwrite(folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE, histImage);
}

bool process(std::string INPUT_IMAGE_PATH)
{
	std::string IMAGE_NAME = INPUT_IMAGE_PATH.substr(INPUT_IMAGE_PATH.find_last_of(FOLDER_PATH_SPLITTER) + 1);
	std::string IMAGE_TYPE = IMAGE_NAME.substr(IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));
	
	IMAGE_NAME = IMAGE_NAME.substr(0, IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));
	const std::string IMAGE_PATH = INPUT_IMAGE_PATH;
	
	Mat in_img = loadImage(IMAGE_PATH);
	Mat out_img(in_img.rows, in_img.cols, CV_8UC3);
	Mat fog_image(in_img.rows, in_img.cols, CV_8UC3);
	
	unsigned char * indata = in_img.data;
	unsigned char * outdata = out_img.data;
	unsigned char * fogData = fog_image.data;

	CHazeRemoval hr;
	hr.InitProc(in_img.cols, in_img.rows, in_img.channels());
	bool result = hr.Process(indata, outdata, fogData, in_img.cols, in_img.rows, in_img.channels());

	if (!result) {
		return false;
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
	std::string date_time = oss.str();
	std::string folderPath = BASE_PATH + OUTPUT_FOLDER_DESTINATION + IMAGE_FOLDER_PREFIX + "[" + IMAGE_NAME + "]" + FILE_NAME_SPACE_DIVIDER + date_time;

	if (mkdir(folderPath.c_str(), 0777) == -1) {
		return false;
	}

	imwrite(folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "original" + IMAGE_TYPE, in_img);
	imwrite(folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "dehaze" + IMAGE_TYPE, out_img);
	imwrite(folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "fog" + IMAGE_TYPE, fog_image);

	processHistogram(in_img, folderPath, IMAGE_NAME, IMAGE_TYPE);

	return true;
}

int main(int argc, char **args) {
	if (argc < 2) {
		std::cerr << "Usage: " << args[0] << " <images_folder>" << std::endl;
		return -1;
	}

	std::string INPUT_FOLDER_PATH = args[1];
	std::vector<std::string> imagePaths = getImagesInFolder(INPUT_FOLDER_PATH);

	for (const auto& INPUT_IMAGE_PATH : imagePaths) {
		bool result = process(INPUT_IMAGE_PATH);
		if(!result) {
			std::cerr << "Error processing image: " << INPUT_IMAGE_PATH << std::endl;
		} else {
			std::cout << "Image processed successfully: " << INPUT_IMAGE_PATH << std::endl;
		}
	}
	
	return 0;
}