#ifndef IMAGE_PIPELINE_H
#define IMAGE_PIPELINE_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using std::cout;
using std::endl;

#pragma region Common Constants
const std::string BASE_PATH = "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/";
const std::string OUTPUT_FOLDER_DESTINATION = "output/";
const std::string IMAGE_FOLDER_PREFIX = "ouput_";
const std::string FILE_TYPE_SPLITTER = ".";
const std::string FOLDER_SPLITTER = "/";
const std::string FOLDER_PATH_SPLITTER = "/\\";
const std::string FILE_NAME_SPACE_DIVIDER = "_";
#pragma endregion

class ImagePipeline
{
public:
    ImagePipeline(int pixel_lower_bound, int histogram_size, float *histogram_range);
    ~ImagePipeline();

public:
    bool ProcessImage(std::string INPUT_IMAGE_PATH, bool saveHistograms, bool saveOriginal, bool saveFogImage);

private:
    int pixel_lower_bound{0};
    int histogram_size{0};
    float *histogram_range{nullptr};
};

#pragma region File System
Mat fs_load_image(const std::string &path);

std::vector<std::string> fs_get_image_paths_from_folder(const std::string &folderPath);
#pragma endregion

#pragma region Histogram
void histogram_draw(Mat &b_hist, Mat &g_hist, Mat &r_hist, std::string savePath, int histogram_size);

void histogram_process(const Mat &image, std::string savePath, int histogram_size, float histogram_range[]);
#pragma endregion

#pragma region Image Processing Pipeline
Mat ipp_alpha_adjust(Mat image, int rows, int cols, int pixelValueAverageUpperBound);

Mat ipp_combine_luminosity_overlay(const Mat &fogImage, const Mat &originalImage, const Mat &alphaMask);
#pragma endregion

#endif // !IMAGE_PIPELINE_H