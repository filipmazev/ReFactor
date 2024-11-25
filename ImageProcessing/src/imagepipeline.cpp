#include "imagepipeline.h"
#include "hazeremoval.h"
#include <sys/stat.h>
#include <filesystem>
#include <cstddef>

using namespace cv;

namespace fs = std::filesystem;

// <summary>
// output_folder | The output folder where the processed images will be saved.
// pixel_lower_bound | Pixel lower bound is the minimum pixel value that will be considered in the image processing.
// histogram_size | Histogram size is the number of bins in the histogram.
// histogram_range | Histogram range is the range of the histogram.
// </summary>
ImagePipeline::ImagePipeline(std::string output_folder, int pixel_lower_bound, int histogram_size, float *histogram_range)
{
    this->output_folder = output_folder;
    this->pixel_lower_bound = pixel_lower_bound;
    this->histogram_size = histogram_size;

    this->histogram_range = new float[histogram_size];
    std::copy(histogram_range, histogram_range + histogram_size, this->histogram_range);
}

ImagePipeline::~ImagePipeline()
{
    delete[] this->histogram_range;
}

// <summary>
// INPUT_IMAGE_PATH | The path of the input image.
// writeHistograms | If true, histograms will be written to the output folder.
// writeOriginal | If true, the original image will be written to the output folder.
// writeFogImage | If true, the fog image will be written to the output folder.
// writeFinalImage | If true, the final image will be written to the output folder.
// </summary>
std::vector<unsigned char> ImagePipeline::ProcessImage(std::string INPUT_IMAGE_PATH, bool writeHistograms, bool writeOriginal, bool writeFogImage, bool writeFinalImage)
{
    std::string IMAGE_NAME = INPUT_IMAGE_PATH.substr(INPUT_IMAGE_PATH.find_last_of(FOLDER_PATH_SPLITTER) + 1);
    std::string IMAGE_TYPE = IMAGE_NAME.substr(IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));

    IMAGE_NAME = IMAGE_NAME.substr(0, IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));
    const std::string IMAGE_PATH = INPUT_IMAGE_PATH;

    Mat in_img = fs_load_image(IMAGE_PATH);

    int rows = in_img.rows;
    int cols = in_img.cols;

    Mat fog_image(rows, cols, CV_8UC3);

    unsigned char *indata = in_img.data;
    unsigned char *fogData = fog_image.data;

    CHazeRemoval hr;

    hr.Init(cols, rows, in_img.channels());
    bool result = hr.Process(indata, fogData, cols, rows, in_img.channels());

    if (!result)
    {
        std::cerr << "Error processing image!" << std::endl;
        return std::vector<unsigned char>();
    }

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
            return std::vector<unsigned char>();
        }
    }

    if(writeOriginal && folderPath != "")
    {
        const std::string original_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "original";
        imwrite(original_image_location + IMAGE_TYPE, in_img);
        
        if(writeHistograms && folderPath != "")
        {
            histogram_process(in_img, original_image_location + FILE_NAME_SPACE_DIVIDER + "histogram" + IMAGE_TYPE, this->histogram_size, this->histogram_range);
        }
    }

    if(writeFogImage && folderPath != "") 
    {
        const std::string fog_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "fog";
        imwrite(fog_image_location + IMAGE_TYPE, fog_image);

        if(writeHistograms)
        {
            histogram_process(fog_image, fog_image_location + "_histogram" + IMAGE_TYPE, this->histogram_size, this->histogram_range);
        }
    }

    const Mat alphaAdjustedFogImage = ipp_alpha_adjust(fog_image, rows, cols, this->pixel_lower_bound);
    Mat finalImage = ipp_combine_luminosity_overlay(fog_image, in_img, alphaAdjustedFogImage);

    if(writeFinalImage && folderPath != "")
    {
        const std::string final_image_location = folderPath + FOLDER_SPLITTER + IMAGE_NAME + FILE_NAME_SPACE_DIVIDER + "final";
        imwrite(final_image_location + IMAGE_TYPE, finalImage);

        if(writeHistograms)
        {
            histogram_process(finalImage, final_image_location + "_histogram" + IMAGE_TYPE, this->histogram_size, this->histogram_range);
        }
    }

    std::vector<uchar> buf;
    imencode(IMAGE_TYPE, finalImage, buf);
    return std::vector<unsigned char>(buf.begin(), buf.end());
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
