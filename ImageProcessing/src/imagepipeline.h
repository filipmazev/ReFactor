#ifndef IMAGE_PIPELINE_H
#define IMAGE_PIPELINE_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <cstddef>

using namespace cv;
using std::cout;
using std::endl;

#pragma region Common Constants
const std::string IMAGE_FOLDER_PREFIX = "ouput_";
const std::string FILE_TYPE_SPLITTER = ".";
const std::string FOLDER_SPLITTER = "/";
const std::string FOLDER_PATH_SPLITTER = "/\\";
const std::string FILE_NAME_SPACE_DIVIDER = "_";
#pragma endregion

#pragma region Structures
typedef struct _per_channel_histogram_metrics {
    double mean;
    double variance;
    double skewness;
} PerChannelHistogramMetrics;

typedef struct _image_histogram_metrics {
    PerChannelHistogramMetrics R;
    PerChannelHistogramMetrics G;
    PerChannelHistogramMetrics B;
} ImageHistogramMetrics;

typedef struct _channel_intensity_ratio {
    double R2G;
    double R2B;
    double G2B;
} ChannelIntensityRatio;
#pragma endregion

class ImagePipeline
{
public:
    ImagePipeline(std::string output_folder, int pixel_lower_bound, int histogram_size, float *histogram_range, int grayLevels);
    ~ImagePipeline();

public:
    std::vector<double> ExtractEnhancedMetadata(std::string INPUT_IMAGE_PATH);

private:
    std::string output_folder {""};

    int pixel_lower_bound{0};
    int histogram_size{0};
    float *histogram_range{nullptr};
    int grayLevels {256};  
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

#pragma region Enhanced Metadata Functions
double calc_fog_impact_index(const Mat &in_img, const Mat &dehazed_image, const Mat &alpha_adjusted_image, int rows, int cols);

PerChannelHistogramMetrics calc_per_channel_histogram_metrics(const Mat& hist, int histogram_size, float histogram_range[]);

ImageHistogramMetrics calc_image_histogram_metrics(const Mat& image, int histogram_size, float histogram_range[]);

ChannelIntensityRatio calc_channel_intensity_ratios(const Mat& image);
#pragma endregion

#pragma region Calculation Functions
double calc_luminosity_delta(const Mat &image_1, const Mat &image_2);

double cal_luminosity(const Mat &image);

double calc_contrast_delta(const Mat image_1, const Mat image_2);

Scalar calc_pixel_intensity_delta(const Mat image_1, const Mat image_2);

int calc_alpha_255_ammount(const Mat image);
#pragma endregion 

#pragma region Data Functions
std::vector<double> extract_features(const ImageHistogramMetrics& hist_metrics, const ChannelIntensityRatio& channel_ratios);
#pragma endregion

#endif // !IMAGE_PIPELINE_H