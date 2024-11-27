#include "program.h"

/// @brief Create a CSV of enhanced metadata from a dataset (requires a csv (that has Filepaths), the full path to your images, a full path to the output folder)
/// @param argc | needs 3 arguments
/// @param args | <input_csv> <images_folder_full_path> <output_folder_full_path>
/// @return 
bool process_CSV_to_enhanced_metadata(int argc, char **args)
{
    if (argc < 3)
    {
        cerr << "Usage: " << args[0] << " <input_csv> <images_folder_full_path> <output_folder_full_path>" << endl;
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