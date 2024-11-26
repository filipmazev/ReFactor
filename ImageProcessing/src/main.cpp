#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include "imagepipeline.h"
#include <algorithm>

namespace fs = std::filesystem;

const std::string OUTPUT_FOLDER_DESTINATION =
#if DEBUG
    "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/output/";
#else
    "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/";
#endif

const std::vector<std::string> NEW_COLUMN_NAMES = {
    "AQI", "AQI_Class", 
    "hour",
    "fog_impact_index", 

    "glcm_energy", "glcm_contrast", "glcm_homogenity", "glcm_entropy",
    
    "airligh_r", "airlight_g", "airlight_b", 
    
    "o_r_mean", "o_r_variance", "o_r_skewness",
    "o_g_mean", "o_g_variance", "o_g_skewness", 
    "o_b_mean", "o_b_variance","o_b_skewness", 
    "o_r2g", "o_r2b", "o_g2b", 
    
    "d_r_mean", "d_r_variance", "d_r_skewness", 
    "d_g_mean", "d_g_variance", "d_g_skewness", 
    "d_b_mean", "d_b_variance", "d_b_skewness"
};

std::string cleanString(const std::string& input) {
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

int main(int argc, char** args) 
{
    if (argc < 3) 
    {
        std::cerr << "Usage: " << args[0] << " <input_csv> <images_folder>" << std::endl;
        return -1;
    }

    std::string INPUT_CSV_PATH = args[1];
    std::string IMAGES_FOLDER_PATH = args[2];

    int image_pixels_w_max = 896;
    int image_pixels_h_max = 896;

    int pixel_lower_bound = 30;
    int histogram_size = 64;
    float histogram_range[2] = {0, 256};
    int grayLevels = 256;

    ImagePipeline pipeline = ImagePipeline(OUTPUT_FOLDER_DESTINATION, image_pixels_w_max, image_pixels_h_max, pixel_lower_bound, histogram_size, histogram_range, grayLevels);

    std::ifstream inputCsv(INPUT_CSV_PATH);

    if (!inputCsv.is_open()) 
    {
        std::cerr << "Error: Unable to open input CSV file." << std::endl;
        return -1;
    }

    std::ofstream outputCsv(OUTPUT_FOLDER_DESTINATION + "processed_features.csv");
    if (!outputCsv.is_open()) 
    {
        std::cerr << "Error: Unable to create output CSV file." << std::endl;
        return -1;
    }

    for (size_t i = 0; i < NEW_COLUMN_NAMES.size(); ++i) 
    {
        outputCsv << NEW_COLUMN_NAMES[i];
        if (i != NEW_COLUMN_NAMES.size() - 1) outputCsv << ",";
    }

    outputCsv << "\n";

    std::string line;
    std::getline(inputCsv, line);

    
    std::cout << "Processing complete. Output saved to 'processed_features.csv'." << std::endl;
    
    std::ostringstream oss;
    
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string startDateTime = oss.str();

    std::cout << "Starting Processing @" << startDateTime << std::endl;

    while (std::getline(inputCsv, line)) 
    {
        std::istringstream ss(line);
        std::string filename, year, month, day, hour, aqi, pm25, pm10, o3, co, so2, no2, aqiClass;

        std::getline(ss, filename, ',');
        std::getline(ss, year, ',');
        std::getline(ss, month, ',');
        std::getline(ss, day, ',');
        std::getline(ss, hour, ',');
        std::getline(ss, aqi, ',');
        std::getline(ss, pm25, ',');
        std::getline(ss, pm10, ',');
        std::getline(ss, o3, ',');
        std::getline(ss, co, ',');
        std::getline(ss, so2, ',');
        std::getline(ss, no2, ',');
        std::getline(ss, aqiClass, ',');

		aqiClass = cleanString(aqiClass); 

        std::string imagePath = IMAGES_FOLDER_PATH + "/" + filename;
       
        if (!fs::exists(imagePath)) 
        {
            std::cerr << "Warning: Image not found: " << imagePath << std::endl;
            continue;
        }

        std::vector<double> metadata = pipeline.ExtractEnhancedMetadata(imagePath);
        if (metadata.empty()) 
        {
            std::cerr << "Error processing image: " << imagePath << std::endl;
            continue;
        } 
        else 
        {
			std::cout << "Processed image: " << imagePath << std::endl;
		}

        outputCsv << aqi << "," << aqiClass << "," << hour << ",";

        for (size_t i = 0; i < metadata.size(); ++i) 
        {
            outputCsv << metadata[i];
            if (i != metadata.size() - 1) outputCsv << ",";
        }

        outputCsv << "\n";
    }

    inputCsv.close();
    outputCsv.close();
    
    t = std::time(nullptr);
    tm = *std::localtime(&t);
    oss = std::ostringstream();

    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string endDateTime = oss.str();

    std::cout << "Processing complete. Output saved to 'processed_features.csv' @" << endDateTime << std::endl;
    
    return 0;
}