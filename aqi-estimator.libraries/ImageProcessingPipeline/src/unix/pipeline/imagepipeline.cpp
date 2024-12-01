///
// Written in 2024 by Filip Mazev (@filipmazev)
//
// To the extent possible under law, the author(s) have dedicated all
// copyright and related and neighboring rights to this software to the
// public domain worldwide. This software is distributed without any warranty.
//
// CC0 Public Domain Dedication <http://creativecommons.org/publicdomain/zero/1.0/>.
///
#include "../includes/pipeline/imagepipeline.h"

// <summary>
// output_folder | The output folder where the processed images will be saved.
// pixel_lower_bound | Pixel lower bound is the minimum pixel value that will be considered in the image processing.
// histogram_size | Histogram size is the number of bins in the histogram.
// histogram_range | Histogram range is the range of the histogram.
// </summary>
ImagePipeline::ImagePipeline(int image_pixels_w_max, int image_pixels_h_max, int pixel_lower_bound, int histogram_size, float *histogram_range, int grayLevels)
{
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
std::vector<double> ImagePipeline::ExtractEnhancedMetadata(cv::Mat &in_img)
{
    int rows = in_img.rows;
    int cols = in_img.cols;

    if (rows > this->image_pixels_h_max || cols > this->image_pixels_w_max)
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
        cerr << "Error processing image!" << endl;
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

    std::vector<double> concatenated_features;
    concatenated_features.push_back(fog_impact_index);

    concatenated_features.insert(concatenated_features.end(), airlightValues.begin(), airlightValues.end());

    concatenated_features.insert(concatenated_features.end(), original_features.begin(), original_features.end());
    concatenated_features.insert(concatenated_features.end(), dehazed_histogram_metrics.begin(), dehazed_histogram_metrics.end());

#ifdef DEBUG
    cout << "----------------------------------------------------------------------------------------------------------------" << endl;

    cout << "Fog Impact Index: " << fog_impact_index << endl;

    cout << "Air Light R: " << airlightR << " Air Light G: " << airlightG << " Air Light B: " << airlightB << endl;

    cout << "Original Image Metrics: " << endl;
    cout << "Red Mean: " << original_features[0] << " Red Variance: " << original_features[1] << " Red Skewness: " << original_features[2] << endl;
    cout << "Green Mean: " << original_features[3] << " Green Variance: " << original_features[4] << " Green Skewness: " << original_features[5] << endl;
    cout << "Blue Mean: " << original_features[6] << " Blue Variance: " << original_features[7] << " Blue Skewness: " << original_features[8] << endl;

    cout << "Red to Green: " << original_features[9] << " Red to Blue: " << original_features[10] << " Green to Blue: " << original_features[11] << endl;

    cout << "Dehazed Image Metrics: " << endl;
    cout << "Red Mean: " << dehazed_histogram_metrics[0] << " Red Variance: " << dehazed_histogram_metrics[1] << " Red Skewness: " << dehazed_histogram_metrics[2] << endl;
    cout << "Green Mean: " << dehazed_histogram_metrics[3] << " Green Variance: " << dehazed_histogram_metrics[4] << " Green Skewness: " << dehazed_histogram_metrics[5] << endl;
    cout << "Blue Mean: " << dehazed_histogram_metrics[6] << " Blue Variance: " << dehazed_histogram_metrics[7] << " Blue Skewness: " << dehazed_histogram_metrics[8] << endl;

    cout << "----------------------------------------------------------------------------------------------------------------" << endl;
#endif

    return concatenated_features;
}

ExtractEnhancedMetadataResult ImagePipeline::ExtractEnhancedMetadataWithTimeTakenResult(cv::Mat &in_img)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<double> data = this->ExtractEnhancedMetadata(in_img);
    auto end = std::chrono::high_resolution_clock::now();

    double time_taken_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    ExtractEnhancedMetadataResult result;
    result.data = data;
    result.time_taken_in_ms = time_taken_in_ms;

    return result;
}

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

PerChannelHistogramMetrics calc_per_channel_histogram_metrics(const Mat &hist, int histogram_size, float histogram_range[])
{
    double sum = 0.0, sum_squared = 0.0, sum_cubed = 0.0;
    double total_pixels = 0.0;

    float bin_width = (histogram_range[1] - histogram_range[0]) / histogram_size;

    for (int i = 0; i < histogram_size; i++)
    {
        float bin_value = hist.at<float>(i);
        float intensity = histogram_range[0] + i * bin_width;

        sum += bin_value * intensity;
        sum_squared += bin_value * intensity * intensity;
        sum_cubed += bin_value * intensity * intensity * intensity;
        total_pixels += bin_value;
    }

    PerChannelHistogramMetrics result;

    result.mean = sum / total_pixels;

    result.variance = (sum_squared / total_pixels) - (result.mean * result.mean);
    result.skewness = ((sum_cubed / total_pixels) - 3 * result.mean * result.variance - (result.mean * result.mean * result.mean)) / std::pow(std::sqrt(result.variance), 3);

    return result;
}

ImageHistogramMetrics calc_image_histogram_metrics(const Mat &image, int histogram_size, float histogram_range[])
{
    std::vector<Mat> bgr_planes;
    split(image, bgr_planes);

    const float *histRange = {histogram_range};
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

ChannelIntensityRatio calc_channel_intensity_ratios(const Mat &image)
{
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
        cerr << "Image does not have an alpha channel!" << endl;
    }

    std::vector<Mat> channels;
    split(image, channels);
    Mat alphaChannel = channels[3];

    int alpha255Count = countNonZero(alphaChannel == 255);

    return alpha255Count;
}
#pragma endregion

#pragma region Data Functions
std::vector<double> extract_all_histogram_features(const ImageHistogramMetrics &hist_metrics, const ChannelIntensityRatio &channel_ratios)
{
    std::vector<double> features = extract_histogram_metrics(hist_metrics);

    features.push_back(channel_ratios.R2G);
    features.push_back(channel_ratios.R2B);
    features.push_back(channel_ratios.G2B);

    return features;
}

std::vector<double> extract_histogram_metrics(const ImageHistogramMetrics &hist_metrics)
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

#pragma region Helper Functions
std::string base64_encode(const std::vector<uchar>& input) 
{
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;
    int val = 0, valb = -6;
    
    for (uchar c : input) 
    {
        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0) 
        {
            output.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) 
    {
        output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (output.size() % 4) 
    {
        output.push_back('=');
    }
    
    return output;
}

std::vector<uchar> base64_decode(const std::string& input) 
{
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uchar> output;
    std::vector<int> T(256, -1);

    for (int i = 0; i < 64; i++) 
    {
        T[base64_chars[i]] = i;
    }
    
    int val = 0, valb = -8;

    for (uchar c : input) 
    {
        if (T[c] == -1) break;

        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) 
        {
            output.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    
    return output;
}
#pragma endregion