#ifndef PROGRAM_H
#define PROGRAM_H

#include "imagepipeline.h"

#include <sys/stat.h>
#include <fstream>

using std::cerr;
using std::cout;
using std::endl;

namespace fs = std::filesystem;

#pragma region Common Constants
const std::vector<std::string> OUTPUT_COLUMN_NAMES = {
    "AQI", "AQI_Class",
    "hour",
    "fog_impact_index",

    "o_glcm_energy", "o_glcm_contrast", "o_glcm_homogenity", "o_glcm_entropy",
    "glcm_energy_delta", "glcm_contrast_delta", "glcm_homogenity_delta", "glcm_entropy_delta",

    "airligh_r", "airlight_g", "airlight_b",

    "o_r_mean", "o_r_variance", "o_r_skewness",
    "o_g_mean", "o_g_variance", "o_g_skewness",
    "o_b_mean", "o_b_variance", "o_b_skewness",
    "o_r2g", "o_r2b", "o_g2b",

    "d_r_mean", "d_r_variance", "d_r_skewness",
    "d_g_mean", "d_g_variance", "d_g_skewness",
    "d_b_mean", "d_b_variance", "d_b_skewness"};
#pragma endregion

bool process_CSV_to_enhanced_metadata(int argc, char **args);

#pragma region File System
std::string h_clean_string(const std::string &input);

Mat fs_load_image(const std::string &path);
#pragma endregion

#pragma region Helper Functions
std::vector<std::string> fs_get_image_paths_from_folder(const std::string &folderPath);
#pragma endregion

#endif // !PROGRAM_H