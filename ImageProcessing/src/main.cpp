#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "imagepipeline.h"

const std::string OUTPUT_FOLDER_DESTINATION =
#if DEBUG
    "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/output/";
#else
    "/Users/filipmazev/Documents/Code/ReFactor/ImageProcessing/";
#endif

const std::vector<std::string> COLUMN_NAMES = {
    "fog_intensity_index", "o_r_mean", "o_r_variance", "o_r_skewness",
    "o_g_mean", "o_g_variance", "o_g_skewness", "o_b_mean", "o_b_variance",
    "o_b_skewness", "o_r2g", "o_r2b", "o_rg2b", "d_r_mean", "d_r_variance",
    "d_r_skewness", "d_g_mean", "d_g_variance", "d_g_skewness", "d_b_mean",
    "d_b_variance", "d_b_skewness", "d_r2g", "d_r2b", "d_rg2b"
};

int main(int argc, char **args) {
    if (argc < 2) {
        std::cerr << "Usage: " << args[0] << " <images_folder>" << std::endl;
        return -1;
    }

    std::string INPUT_FOLDER_PATH = args[1];
    std::vector<std::string> imagePaths = fs_get_image_paths_from_folder(INPUT_FOLDER_PATH);

    int pixel_lower_bound = 30;
    int histogram_size = 64;
    float histogram_range[2] = {0, 256};

    ImagePipeline pipeline = ImagePipeline(OUTPUT_FOLDER_DESTINATION, pixel_lower_bound, histogram_size, histogram_range);

    std::ofstream csvFile(OUTPUT_FOLDER_DESTINATION + "features_output.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Error: Unable to create CSV file." << std::endl;
        return -1;
    }

    for (size_t i = 0; i < COLUMN_NAMES.size(); ++i) {
        csvFile << COLUMN_NAMES[i];
        if (i != COLUMN_NAMES.size() - 1) csvFile << ",";
    }
    csvFile << "\n";

    for (const auto& INPUT_IMAGE_PATH : imagePaths) {
        std::vector<double> result = pipeline.ExtractEnhancedMetadata(INPUT_IMAGE_PATH);
        if (result.empty()) {
            std::cerr << "Error processing image: " << INPUT_IMAGE_PATH << std::endl;
        } else {
            std::cout << "Image processed successfully: " << INPUT_IMAGE_PATH << std::endl;

            for (size_t i = 0; i < result.size(); ++i) {
                csvFile << result[i];
                if (i != result.size() - 1) csvFile << ",";
            }
            csvFile << "\n";
        }
    }

    csvFile.close();
    return 0;
}