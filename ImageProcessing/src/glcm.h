#ifndef GLCM_H
#define GLCM_H

#include "opencv2/opencv.hpp"
#include <vector>

const std::vector<std::pair<int, int>> directions = {
    {0, 1},  // 0 degrees (horizontal)
    {-1, 1}, // 45 degrees
    {-1, 0}, // 90 degrees (vertical)
    {-1, -1} // 135 degrees
};

void calc_GLCM_parallel(const cv::Mat &image, int grayLevels, std::vector<double> &contrast, std::vector<double> &energy);

#endif // GLCM_H