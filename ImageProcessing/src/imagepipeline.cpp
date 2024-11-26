#include "imagepipeline.h"
#include "hazeremoval.h"
#include "glcm.h"
#include <sys/stat.h>
#include <filesystem>
#include <cstddef>
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/face.hpp>

using namespace cv;

namespace fs = std::filesystem;

// <summary>
// output_folder | The output folder where the processed images will be saved.
// pixel_lower_bound | Pixel lower bound is the minimum pixel value that will be considered in the image processing.
// histogram_size | Histogram size is the number of bins in the histogram.
// histogram_range | Histogram range is the range of the histogram.
// </summary>
ImagePipeline::ImagePipeline(std::string output_folder, int image_pixels_w_max, int image_pixels_h_max, int pixel_lower_bound, int histogram_size, float *histogram_range, int grayLevels)
{
    this->output_folder = output_folder;

    this->image_pixels_w_max = image_pixels_w_max;
    this->image_pixels_h_max = image_pixels_h_max;

    this->pixel_lower_bound = pixel_lower_bound;
    this->histogram_size = histogram_size;

    this->histogram_range = new float[histogram_size];
    std::copy(histogram_range, histogram_range + histogram_size, this->histogram_range);

    this->grayLevels = grayLevels;
}

ImagePipeline::~ImagePipeline()
{
    delete[] this->histogram_range;
}

// <summary>
// INPUT_IMAGE_PATH | The path of the input image.
// </summary>
std::vector<double> ImagePipeline::ExtractEnhancedMetadata(std::string INPUT_IMAGE_PATH)
{
    std::string IMAGE_NAME = INPUT_IMAGE_PATH.substr(INPUT_IMAGE_PATH.find_last_of(FOLDER_PATH_SPLITTER) + 1);
    std::string IMAGE_TYPE = IMAGE_NAME.substr(IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));

    IMAGE_NAME = IMAGE_NAME.substr(0, IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));
    const std::string IMAGE_PATH = INPUT_IMAGE_PATH;

    Mat in_img = fs_load_image(IMAGE_PATH);
    
    int rows = in_img.rows;
    int cols = in_img.cols;

    if(rows > this->image_pixels_h_max || cols > this->image_pixels_w_max)
    {
        resize(in_img, in_img, Size(std::min(this->image_pixels_w_max, in_img.rows), std::min(this->image_pixels_h_max, in_img.cols)));
        rows = in_img.rows;
        cols = in_img.cols;
    }

    Mat dehazed_image(rows, cols, CV_8UC3);
    Mat fog_image(rows, cols, CV_8UC3);

    unsigned char *indata = in_img.data;
    unsigned char *outdata = dehazed_image.data;
    unsigned char *fogData = fog_image.data;
    
    Vec3d airlight;

    CHazeRemoval hr;

    hr.Init(cols, rows, in_img.channels());
    bool result = hr.Process(indata, outdata, fogData, airlight, cols, rows, in_img.channels());

    if (!result)
    {
        std::cerr << "Error processing image!" << std::endl;
        return std::vector<double>();
    }

    const Mat alpha_adjusted_image = ipp_alpha_adjust(fog_image, rows, cols, this->pixel_lower_bound);

    double fog_impact_index = calc_fog_impact_index(in_img, dehazed_image, alpha_adjusted_image, rows, cols);

    double airlightR = airlight[0];
    double airlightG = airlight[1];
    double airlightB = airlight[2];

    std::vector<double> airlightValues = {airlightR, airlightG, airlightB};

    ImageHistogramMetrics original_image_histogram_metrics = calc_image_histogram_metrics(in_img, this->histogram_size, this->histogram_range);
    ChannelIntensityRatio original_channel_intensity_ratios = calc_channel_intensity_ratios(in_img);
    std::vector<double> original_features = extract_all_histogram_features(original_image_histogram_metrics, original_channel_intensity_ratios);

    ImageHistogramMetrics dehaze_image_histogram_metrics = calc_image_histogram_metrics(dehazed_image, this->histogram_size, this->histogram_range);
    std::vector<double> dehazed_histogram_metrics = extract_histogram_metrics(dehaze_image_histogram_metrics);

    GLCMFeatures original_glcm = calc_GLCM_parallel(in_img, this->grayLevels);
    std::vector<double> original_texture_eigenvalues = {original_glcm.energy, original_glcm.contrast, original_glcm.homogenity, original_glcm.entropy};

    GLCMFeatures fog_glcm = calc_GLCM_parallel(fog_image, this->grayLevels);
    std::vector<double> fog_texture_eigenvalues = {fog_glcm.energy, fog_glcm.contrast, fog_glcm.homogenity, fog_glcm.entropy};

    double energy_delta = fog_glcm.energy - original_glcm.energy;
    double contrast_delta = fog_glcm.contrast - original_glcm.contrast;
    double homogenity_delta = fog_glcm.homogenity - original_glcm.homogenity;
    double entropy_delta = fog_glcm.entropy - original_glcm.entropy;

    std::vector<double> texture_eigenvalues_delta = {energy_delta, contrast_delta, homogenity_delta, entropy_delta};

    std::vector<double> concatenated_features;
    concatenated_features.push_back(fog_impact_index);

    concatenated_features.insert(concatenated_features.end(), original_texture_eigenvalues.begin(), original_texture_eigenvalues.end());
    concatenated_features.insert(concatenated_features.end(), texture_eigenvalues_delta.begin(), texture_eigenvalues_delta.end());

    concatenated_features.insert(concatenated_features.end(), airlightValues.begin(), airlightValues.end());

    concatenated_features.insert(concatenated_features.end(), original_features.begin(), original_features.end());
    concatenated_features.insert(concatenated_features.end(), dehazed_histogram_metrics.begin(), dehazed_histogram_metrics.end());
  
#ifdef DEBUG

    std::cout << "----------------------------------------------------------------------------------------------------------------" << std::endl;

    std::cout << "Image Name: " << IMAGE_NAME << std::endl << std::endl;

    std::cout << "Fog Impact Index: " << fog_impact_index << std::endl;
    
    std::cout << "Initial Image Texture Eigenvalues: " << std::endl;
    std::cout << "Energy: " << original_glcm.energy << " Contrast: " << original_glcm.contrast << " Homogenity: " << original_glcm.homogenity << " Entropy: " << original_glcm.entropy << std::endl;

    std::cout << "Texture Eigenvalues Delta: " << std::endl;
    std::cout << "Energy: " << energy_delta << " Contrast: " << contrast_delta << " Homogenity: " << homogenity_delta << " Entropy: " << entropy_delta << std::endl;

    std::cout << "Air Light R: " << airlightR << " Air Light G: " << airlightG <<  " Air Light B: " << airlightB << std::endl;

    std::cout << "Original Image Metrics: " << std::endl; 
    std::cout << "Red Mean: " << original_features[0] << " Red Variance: " << original_features[1] << " Red Skewness: " << original_features[2] << std::endl;
    std::cout << "Green Mean: " << original_features[3] << " Green Variance: " << original_features[4] << " Green Skewness: " << original_features[5] << std::endl;
    std::cout << "Blue Mean: " << original_features[6] << " Blue Variance: " << original_features[7] << " Blue Skewness: " << original_features[8] << std::endl;

    std::cout << "Red to Green: " << original_features[9] << " Red to Blue: " << original_features[10] << " Green to Blue: " << original_features[11] << std::endl;
   
    std::cout << "Dehazed Image Metrics: " << std::endl;
    std::cout << "Red Mean: " << dehazed_histogram_metrics[0] << " Red Variance: " << dehazed_histogram_metrics[1] << " Red Skewness: " << dehazed_histogram_metrics[2] << std::endl;
    std::cout << "Green Mean: " << dehazed_histogram_metrics[3] << " Green Variance: " << dehazed_histogram_metrics[4] << " Green Skewness: " << dehazed_histogram_metrics[5] << std::endl;
    std::cout << "Blue Mean: " << dehazed_histogram_metrics[6] << " Blue Variance: " << dehazed_histogram_metrics[7] << " Blue Skewness: " << dehazed_histogram_metrics[8] << std::endl;

    std::cout << "----------------------------------------------------------------------------------------------------------------" << std::endl;

    std::string folderPath = "";

    if(this->output_folder != "")
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        std::string date_time = oss.str();
    
        folderPath = this->output_folder + IMAGE_FOLDER_PREFIX + "[" + IMAGE_NAME + "]" + FILE_NAME_SPACE_DIVIDER + date_time;

        if (mkdir(folderPath.c_str(), 0777) == -1)
        {
            std::cerr << "Error creating output folder!" << std::endl;
            return std::vector<double>();
        }
    }

    const std::string original_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "original";
    imwrite(original_image_location + IMAGE_TYPE, in_img);

    const std::string dehazed_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "dehazed";
    imwrite(dehazed_image_location + IMAGE_TYPE, dehazed_image);

    const std::string fog_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "fog";
    imwrite(fog_image_location + IMAGE_TYPE, fog_image);

    Mat finalImage = ipp_combine_luminosity_overlay(fog_image, in_img, alpha_adjusted_image);

    const std::string final_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "final";
    imwrite(final_image_location + IMAGE_TYPE, finalImage);
    #endif

    return concatenated_features;
}

#pragma region File System
Mat fs_load_image(const std::string &path)
{
    Mat image = imread(path, IMREAD_COLOR);
    if (image.empty())
    {
        std::cerr << "Error loading image!" << std::endl;
        exit(-1);
    }
    return image;
}

std::vector<std::string> fs_get_image_paths_from_folder(const std::string &folderPath)
{
    std::vector<std::string> imagePaths;
    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            std::string path = entry.path().string();
            if (path.find(".jpg") != std::string::npos || path.find(".png") != std::string::npos || path.find(".jpeg") != std::string::npos || path.find(".bmp") != std::string::npos || path.find(".JPEG") != std::string::npos)
            {
                imagePaths.push_back(path);
            }
        }
    }
    return imagePaths;
}
#pragma endregion

#pragma region Histogram
void histogram_draw(Mat &b_hist, Mat &g_hist, Mat &r_hist, std::string savePath, int histogram_size)
{
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histogram_size);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1,
              Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1,
              Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1,
              Mat());

    for (int i = 1; i < histogram_size; i++)
    {
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

void histogram_process(const Mat &image, std::string savePath, int histogram_size, float histogram_range[])
{
    std::vector<Mat> bgr_planes;
    split(image, bgr_planes);

    const float *histRange = {histogram_range};

    bool uniform = true;
    bool accumulate = false;

    Mat b_hist, g_hist, r_hist;

    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histogram_size,
             &histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histogram_size,
             &histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histogram_size,
             &histRange, uniform, accumulate);

    histogram_draw(b_hist, g_hist, r_hist, savePath, histogram_size);
}
#pragma endregion

#pragma region Image Processing Pipeline
Mat ipp_alpha_adjust(Mat image, int rows, int cols, int pixelValueAverageUpperBound)
{
    Mat outputWithAlpha(rows, cols, CV_8UC4);
    cvtColor(image, outputWithAlpha, COLOR_BGR2BGRA);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Vec4b &pixel = outputWithAlpha.at<Vec4b>(i, j);
            int avg = (pixel[0] + pixel[1] + pixel[2]) / 3;
            if (avg >= 0 && avg <= pixelValueAverageUpperBound)
            {
                pixel[3] = 0;
            }
            else
            {
                pixel[3] = 255;
            }
        }
    }
    
    return outputWithAlpha;
}

Mat ipp_combine_luminosity_overlay(const Mat &image, const Mat &originalImage, const Mat &alphaMask)
{
    Mat luminosity;
    cvtColor(originalImage, luminosity, COLOR_BGR2GRAY);

    Mat result = image.clone();

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            uchar alpha = alphaMask.at<Vec4b>(i, j)[3];
            if (alpha > 0)
            {
                Vec3b &fogPixel = result.at<Vec3b>(i, j);
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
#pragma endregion

#pragma region Enhanced Metadata Functions
double calc_fog_impact_index(const Mat &in_img, const Mat &dehazed_image, const Mat &alpha_adjusted_image, int rows, int cols)
{
    int alpha255Count = calc_alpha_255_ammount(alpha_adjusted_image);
    double fog_coverage = (double)alpha255Count / (rows * cols);
    double luminosity_delta = calc_luminosity_delta(in_img, dehazed_image);
    double normalized_contrast_improvement = calc_contrast_delta(in_img, dehazed_image) / 255.0;
    Scalar pixel_intesity_delta = calc_pixel_intensity_delta(in_img, dehazed_image);

    double normalized_intensity_change = (pixel_intesity_delta[0] + pixel_intesity_delta[1] + pixel_intesity_delta[2]) / (255.0 * 3 * rows * cols);
    
    double fog_impact_index = normalized_intensity_change + normalized_contrast_improvement + fog_coverage;

    return fog_impact_index;
}

PerChannelHistogramMetrics calc_per_channel_histogram_metrics(const Mat& hist, int histogram_size, float histogram_range[]) {
    double sum = 0.0, sum_squared = 0.0, sum_cubed = 0.0;
    double total_pixels = 0.0;

    float bin_width = (histogram_range[1] - histogram_range[0]) / histogram_size;

    for (int i = 0; i < histogram_size; i++) {
        float bin_value = hist.at<float>(i);
        float intensity = histogram_range[0] + i * bin_width;

        sum += bin_value * intensity;
        sum_squared += bin_value * intensity * intensity;
        sum_cubed += bin_value * intensity * intensity * intensity;
        total_pixels += bin_value;
    }

    PerChannelHistogramMetrics result;

    result.mean = sum / total_pixels;

    result.variance = (sum_squared / total_pixels) - (result.mean  * result.mean );
    result.skewness = ((sum_cubed / total_pixels) - 3 * result.mean  * result.variance - (result.mean  * result.mean  * result.mean )) / std::pow(std::sqrt(result.variance), 3);

    return result;
}

ImageHistogramMetrics calc_image_histogram_metrics(const Mat& image, int histogram_size, float histogram_range[]) 
{
    std::vector<Mat> bgr_planes;
    split(image, bgr_planes);

    const float* histRange = {histogram_range};
    bool uniform = true, accumulate = false;

    Mat b_hist, g_hist, r_hist;

    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histogram_size, &histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histogram_size, &histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histogram_size, &histRange, uniform, accumulate);

    ImageHistogramMetrics result;

    result.R = calc_per_channel_histogram_metrics(r_hist, histogram_size, histogram_range);
    result.G = calc_per_channel_histogram_metrics(g_hist, histogram_size, histogram_range);
    result.B = calc_per_channel_histogram_metrics(b_hist, histogram_size, histogram_range);

    return result;
}

ChannelIntensityRatio calc_channel_intensity_ratios(const Mat& image) {
    std::vector<Mat> bgr_planes;
    split(image, bgr_planes);

    Scalar mean_r = mean(bgr_planes[2]);
    Scalar mean_g = mean(bgr_planes[1]);
    Scalar mean_b = mean(bgr_planes[0]);

    ChannelIntensityRatio result;

    result.R2G = mean_r[0] / mean_g[0];
    result.R2B = mean_r[0] / mean_b[0];
    result.G2B = mean_g[0] / mean_b[0];

    return result;
}
#pragma endregion

#pragma region Calculation Functions
double calc_luminosity_delta(const Mat &image_1, const Mat &image_2)
{
    Mat grayImage1, grayImage2;
    cvtColor(image_1, grayImage1, COLOR_BGR2GRAY);
    cvtColor(image_2, grayImage2, COLOR_BGR2GRAY);

    Scalar meanLuminosity1 = mean(grayImage1);
    Scalar meanLuminosity2 = mean(grayImage2);

    return meanLuminosity1[0] - meanLuminosity2[0];
}

double cal_luminosity(const Mat &image)
{
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    Scalar meanLuminosity = mean(grayImage);
    return meanLuminosity[0];
}

double calc_contrast_delta(const Mat image_1, const Mat image_2)
{
    Mat grayOriginal, grayDehazed;
    cvtColor(image_1, grayOriginal, COLOR_BGR2GRAY);
    cvtColor(image_2, grayDehazed, COLOR_BGR2GRAY);

    Scalar image_1_contrast = mean(grayOriginal);
    Scalar image_2_contrast = mean(grayDehazed);

    double contrast_delta = image_2_contrast[0] - image_1_contrast[0];
    return contrast_delta;
}

Scalar calc_pixel_intensity_delta(const Mat image_1, const Mat image_2)
{
    Mat diff;
    absdiff(image_1, image_2, diff);
    Scalar totalDiff = sum(diff);
    return totalDiff;
}

int calc_alpha_255_ammount(const Mat image) 
{
    if (image.channels() != CHANNEL_REQUIREMENT + 1) 
    {
        std::cerr << "Image does not have an alpha channel!" << std::endl;
    }
 
    std::vector<Mat> channels;
    split(image, channels);
    Mat alphaChannel = channels[3];

    int alpha255Count = countNonZero(alphaChannel == 255);

    return alpha255Count;
}
#pragma endregion

#pragma region Data Functions
std::vector<double> extract_all_histogram_features(const ImageHistogramMetrics& hist_metrics, const ChannelIntensityRatio& channel_ratios) 
{
    std::vector<double> features = extract_histogram_metrics(hist_metrics);

    features.push_back(channel_ratios.R2G);
    features.push_back(channel_ratios.R2B);
    features.push_back(channel_ratios.G2B);

    return features;
}

std::vector<double> extract_histogram_metrics(const ImageHistogramMetrics& hist_metrics) 
{
    std::vector<double> features;

    features.push_back(hist_metrics.R.mean);
    features.push_back(hist_metrics.R.variance);
    features.push_back(hist_metrics.R.skewness);
    features.push_back(hist_metrics.G.mean);
    features.push_back(hist_metrics.G.variance);
    features.push_back(hist_metrics.G.skewness);
    features.push_back(hist_metrics.B.mean);
    features.push_back(hist_metrics.B.variance);
    features.push_back(hist_metrics.B.skewness);

    return features;
}
#pragma endregion