///
// Written in 2024 by Filip Mazev (@filipmazev)
//
// To the extent possible under law, the author(s) have dedicated all
// copyright and related and neighboring rights to this software to the
// public domain worldwide. This software is distributed without any warranty.
//
// CC0 Public Domain Dedication <http://creativecommons.org/publicdomain/zero/1.0/>.
///
#include "../includes/pipeline/program.h"

/// @brief Create a CSV of enhanced metadata from a dataset (requires a csv (that has Filepaths), the full path to your images, a full path to the output folder)
/// @param argc | needs 3 arguments
/// @param args | <input_csv_full_path> <images_folder_full_path> <output_folder_full_path>
/// @return 
bool process_CSV_to_enhanced_metadata(int argc, char **args)
{
    if (argc < 3)
    {
        cerr << "Usage: " << args[0] << " <input_csv_full_path> <images_folder_full_path> <output_folder_full_path>" << endl;
        return false;
    }

    std::string INPUT_CSV_PATH = args[1];
    std::string IMAGES_FOLDER_PATH = args[2];
    std::string OUTPUT_FOLDER_DESTINATION = args[3];

    int image_pixels_w_max = 896;
    int image_pixels_h_max = 896;

    int pixel_lower_bound = 30;
    int histogram_size = 64;
    float histogram_range[2] = {0, 256};
    int grayLevels = 256;

    ImagePipeline pipeline = ImagePipeline(image_pixels_w_max, image_pixels_h_max, pixel_lower_bound, histogram_size, histogram_range, grayLevels);

    std::ifstream inputCsv(INPUT_CSV_PATH);

    if (!inputCsv.is_open())
    {
        cerr << "Error: Unable to open input CSV file." << endl;
        return false;
    }

    std::ofstream outputCsv(OUTPUT_FOLDER_DESTINATION + "processed_features.csv");
    if (!outputCsv.is_open())
    {
        cerr << "Error: Unable to create output CSV file." << endl;
        return false;
    }

    for (size_t i = 0; i < OUTPUT_COLUMN_NAMES.size(); ++i)
    {
        outputCsv << OUTPUT_COLUMN_NAMES[i];
        if (i != OUTPUT_COLUMN_NAMES.size() - 1)
            outputCsv << ",";
    }

    outputCsv << "\n";

    std::string line;
    std::getline(inputCsv, line);

    std::ostringstream oss;

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string startDateTime = oss.str();

    cout << "Starting Processing @" << startDateTime << endl;

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

        aqiClass = h_clean_string(aqiClass);

        std::string imagePath = IMAGES_FOLDER_PATH + "/" + filename;

        if (!fs::exists(imagePath))
        {
            cerr << "Warning: Image not found: " << imagePath << endl;
            continue;
        }

        std::string IMAGE_NAME = imagePath.substr(imagePath.find_last_of(FOLDER_PATH_SPLITTER) + 1);
        std::string IMAGE_TYPE = IMAGE_NAME.substr(IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));

        IMAGE_NAME = IMAGE_NAME.substr(0, IMAGE_NAME.find_last_of(FILE_TYPE_SPLITTER));

        Mat in_img = fs_load_image(imagePath);

        std::vector<double> metadata = pipeline.ExtractEnhancedMetadata(in_img);
        if (metadata.empty())
        {
            cerr << "Error processing image: " << imagePath << endl;
            continue;
        }
        else
        {
            cout << "Processed image: " << imagePath << endl;
        }

        outputCsv << aqi << "," << aqiClass << "," << hour << ",";

        for (size_t i = 0; i < metadata.size(); ++i)
        {
            outputCsv << metadata[i];
            if (i != metadata.size() - 1)
                outputCsv << ",";
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

    cout << "Processing complete. Output saved to 'processed_features.csv' @" << endDateTime << endl;

    return true;
}

/// @brief Extract enhanced metadata from a byte array image
/// @param argc | needs 1 argument
/// @param args | <input_byte_array_txt_file_path>
/// @return 
bool process_byteArray_to_enhanced_metadata(int argc, char **args)
{
    if (argc < 2) 
    {
        std::cerr << "Usage: " << args[0] << " <input_byte_array_txt_file_path>" << std::endl;
        return false;
    }

    std::string INPUT_BYTE_ARRAY_TXT_FILE_PATH = args[1];

    std::ifstream inputFile(INPUT_BYTE_ARRAY_TXT_FILE_PATH);
    if (!inputFile.is_open())
    {
        std::cerr << "Error: Unable to open input file." << std::endl;
        return false;
    }

    std::string INPUT_BYTE_ARRAY_IMAGE;
    std::getline(inputFile, INPUT_BYTE_ARRAY_IMAGE);
    
    std::vector<uchar> byteArray = base64_decode(INPUT_BYTE_ARRAY_IMAGE);
    
    cv::Mat in_img = cv::imdecode(byteArray, cv::IMREAD_COLOR); 
    if (in_img.empty())
    {
        std::cerr << "Failed to decode image from byte array." << std::endl;
        return false;
    }

    int image_pixels_w_max = 896;
    int image_pixels_h_max = 896;
    int pixel_lower_bound = 30;
    int histogram_size = 64;
    float histogram_range[2] = {0, 256};
    int grayLevels = 256;

    ImagePipeline pipeline(image_pixels_w_max, image_pixels_h_max, pixel_lower_bound, histogram_size, histogram_range, grayLevels);

    std::vector<double> data = pipeline.ExtractEnhancedMetadata(in_img);

    for (size_t i = 0; i < data.size(); ++i)
    {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;

    return true;
}

/// @brief Convert an image to a byte array and save it to a file
/// @param argc | needs 2 arguments 
/// @param args | <image_path> <output_path>
/// @return 
bool process_image_into_byte_array(int argc, char **args)
{
    if (argc < 3) 
    {
        std::cerr << "Usage: " << args[0] << " <image_path> <output_path>" << std::endl;
        return false;
    }

    std::string image_path = args[1];
    std::string output_path = args[2];

    std::string imageType = image_path.substr(image_path.find_last_of('.') + 1);
    cv::Mat image = fs_load_image(image_path);

    if (image.empty()) 
    {
        std::cerr << "Error: Failed to read image!" << std::endl;
        return false;
    }

    std::vector<uchar> byteArray;
    bool success = cv::imencode(".jpg", image, byteArray);  
    if (!success) 
    {
        std::cerr << "Error: Failed to encode image!" << std::endl;
        return false;
    }

    std::ofstream outputFile(output_path + "/output_byte_array.txt");
    
    if (!outputFile.is_open()) 
    {
        std::cerr << "Error: Unable to create output file!" << std::endl;
        return false;
    }

    std::string base64_str = base64_encode(byteArray);
    
    outputFile << base64_str;
    outputFile.close();

    return true;
}

#pragma region File System
Mat fs_load_image(const std::string &path)
{
    Mat image = imread(path, IMREAD_COLOR);
    if (image.empty())
    {
        cerr << "Error loading image!" << endl;
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

#pragma region Helper Functions
std::string h_clean_string(const std::string &input)
{
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}
#pragma endregion