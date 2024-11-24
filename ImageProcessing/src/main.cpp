#include "hazeremoval.h"
#include "other/arr.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <sys/types.h>
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

const int histSize = 256;

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

void drawHistogram(Mat& b_hist,Mat& g_hist,Mat& r_hist, std::string savePath) 
{
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1,
                  Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1,
                  Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1,
                  Mat());

    for (int i = 1; i < histSize; i++) {
      line(
          histImage,
          Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
          Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
          Scalar(255, 0, 0), 2, 8, 0);
      line(
          histImage,
          Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
          Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
          Scalar(0, 255, 0), 2, 8, 0);
      line(
          histImage,
          Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
          Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
          Scalar(0, 0, 255), 2, 8, 0);
    }

	imwrite(savePath, histImage);
}

void processHistogram(const Mat& image, std::string savePath) 
{
	std::vector<Mat> bgr_planes;
    split(image, bgr_planes);

 	float range[] = {0, 256};
    const float *histRange = {range};

    bool uniform = true;
    bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize,
                 &histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize,
                 &histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize,
                 &histRange, uniform, accumulate);

	drawHistogram(b_hist, g_hist, r_hist, savePath);
}

Mat alphaAdjustedImage(std::string SAVE_LOCATION, Mat fog_image, int rows, int cols, int pixelValueAverageUpperBound) 
{
	Mat outputWithAlpha(rows, cols, CV_8UC4); 
    cvtColor(fog_image, outputWithAlpha, COLOR_BGR2BGRA); 

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec4b& pixel = outputWithAlpha.at<Vec4b>(i, j);
            int avg = (pixel[0] + pixel[1] + pixel[2]) / 3; 
            if (avg >= 0 && avg <= pixelValueAverageUpperBound) {
                pixel[3] = 0; 
            } else {
                pixel[3] = 255; 
            }
        }
    }

	imwrite(SAVE_LOCATION, outputWithAlpha);

	return outputWithAlpha;
}

Mat applyAlphaMask(const Mat& originalImage, const Mat& alphaImage) {
    // Ensure both images have the same dimensions
    if (originalImage.rows != alphaImage.rows || originalImage.cols != alphaImage.cols) {
        std::cerr << "Error: Image dimensions do not match!" << std::endl;
        return Mat();
    }

    // Convert the original image to BGRA if it isn't already
    Mat originalWithAlpha;
    cvtColor(originalImage, originalWithAlpha, COLOR_BGR2BGRA);

    // Extract the alpha channel from alphaImage
    std::vector<Mat> alphaChannels;
    split(alphaImage, alphaChannels); // alphaChannels[3] contains the alpha channel

    if (alphaChannels.size() < 4) {
        std::cerr << "Error: Alpha image is not in BGRA format!" << std::endl;
        return Mat();
    }

    // Replace the alpha channel in original image with the alpha channel from alphaImage
    std::vector<Mat> originalChannels;
    split(originalWithAlpha, originalChannels); // [B, G, R, A]

    // Set the alpha channel
    originalChannels[3] = alphaChannels[3];

    // Merge back to create the final RGBA image
    merge(originalChannels, originalWithAlpha);

    return originalWithAlpha;
}

bool process(std::string INPUT_IMAGE_PATH)
{
	std::string IMAGE_NAME = INPUT_IMAGE_PATH.substr(INPUT_IMAGE_PATH.find_last_of(FOLDER_PATH_SPLITTER) + 1);
	std::string IMAGE_TYPE = IMAGE_NAME.substr(IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));
	
	IMAGE_NAME = IMAGE_NAME.substr(0, IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));
	const std::string IMAGE_PATH = INPUT_IMAGE_PATH;
	
	Mat in_img = loadImage(IMAGE_PATH);
	
	int rows = in_img.rows;
	int cols = in_img.cols;

	Mat out_img(rows, cols, CV_8UC3);
	Mat fog_image(rows, cols, CV_8UC3);
	
	unsigned char * indata = in_img.data;
	unsigned char * outdata = out_img.data;
	unsigned char * fogData = fog_image.data;

	CHazeRemoval hr;
	hr.InitProc(cols, rows, in_img.channels());
	bool result = hr.Process(indata, outdata, fogData, cols, rows, in_img.channels());

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

	const std::string original_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "original";
	const std::string dehaze_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "dehaze";
	const std::string fog_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "fog";
	const std::string alpha_adjusted_fog_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "fog_w_alpha";
	const std::string alpha_masked_original_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "original_w_alpha";

	const Mat alphaAdjustedFogImage = alphaAdjustedImage(alpha_adjusted_fog_image_location + ".png", fog_image, rows, cols, 30);
	const Mat alphaMaskedOriginal = applyAlphaMask(in_img, alphaAdjustedFogImage);

	imwrite(original_image_location + IMAGE_TYPE, in_img);
	imwrite(dehaze_image_location + IMAGE_TYPE, out_img);
	imwrite(fog_image_location + IMAGE_TYPE, fog_image);
	imwrite(alpha_masked_original_image_location + ".png", alphaMaskedOriginal);

	processHistogram(in_img, original_image_location + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE);
	processHistogram(out_img, dehaze_image_location + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE);
	processHistogram(fog_image, fog_image_location + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE);
	processHistogram(alphaMaskedOriginal, alpha_masked_original_image_location + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE);

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