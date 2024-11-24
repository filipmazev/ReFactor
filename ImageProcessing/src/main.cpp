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

const int blackPixelValueUpperBound = 30;
const int blurKernelSize = 25; // Must be an odd number for GaussianBlur

const int histSize = 64;

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

Mat alphaAdjustedImage(Mat fog_image, int rows, int cols, int pixelValueAverageUpperBound) 
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

	return outputWithAlpha;
}

Mat combineLuminosityOverlay(const Mat& fogImage, const Mat& originalImage, const Mat& alphaMask) {
    Mat luminosity;
    cvtColor(originalImage, luminosity, COLOR_BGR2GRAY); 

    Mat result = fogImage.clone();

    for (int i = 0; i < fogImage.rows; i++) {
        for (int j = 0; j < fogImage.cols; j++) {
            uchar alpha = alphaMask.at<Vec4b>(i, j)[3]; 
            if (alpha > 0) {
                Vec3b& fogPixel = result.at<Vec3b>(i, j);
                uchar fogLuminosity = static_cast<uchar>(0.3 * fogPixel[2] + 0.59 * fogPixel[1] + 0.11 * fogPixel[0]);
                uchar newLuminosity = luminosity.at<uchar>(i, j);

                float luminosityFactor = (float)newLuminosity / std::max((float)fogLuminosity, 1.0f);
                fogPixel[0] = cv::saturate_cast<uchar>(fogPixel[0] * luminosityFactor); 
                fogPixel[1] = cv::saturate_cast<uchar>(fogPixel[1] * luminosityFactor); 
                fogPixel[2] = cv::saturate_cast<uchar>(fogPixel[2] * luminosityFactor);
            }
        }
    }
    return result;
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

    Mat fog_image(rows, cols, CV_8UC3);
    
    unsigned char * indata = in_img.data;
    unsigned char * fogData = fog_image.data;

    CHazeRemoval hr;
    hr.InitProc(cols, rows, in_img.channels());
    bool result = hr.Process(indata, fogData, cols, rows, in_img.channels());

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
    imwrite(original_image_location + IMAGE_TYPE, in_img);
    processHistogram(in_img, original_image_location + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE);

    const Mat alphaAdjustedFogImage = alphaAdjustedImage(fog_image, rows, cols, blackPixelValueUpperBound);
    Mat finalImage = combineLuminosityOverlay(fog_image, in_img, alphaAdjustedFogImage);

    Mat blurredImage;
    GaussianBlur(finalImage, blurredImage, Size(blurKernelSize, blurKernelSize), 0);

    const std::string final_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "final";
    imwrite(final_image_location + IMAGE_TYPE, finalImage);
    processHistogram(finalImage, final_image_location + "_histogram" + IMAGE_TYPE);

    const std::string blurred_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "final_blurred";
    imwrite(blurred_image_location + IMAGE_TYPE, blurredImage);
    processHistogram(blurredImage, blurred_image_location + "_histogram" + IMAGE_TYPE);

    return true;
}

int main(int argc, char **args) 
{
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