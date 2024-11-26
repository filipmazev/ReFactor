#include "glcm.h"

void calc_GLCM_parallel(const cv::Mat &image, int grayLevels, std::vector<double> &contrast, std::vector<double> &energy)
{
    const int rows = image.rows;
    const int cols = image.cols;
    const size_t numDirections = directions.size();

    contrast.resize(numDirections, 0.0);
    energy.resize(numDirections, 0.0);

    std::vector<cv::Mat> threadLocalGLCMs(numDirections, cv::Mat::zeros(grayLevels, grayLevels, CV_32F));

#pragma omp parallel
    {
        #pragma omp for
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < cols; ++x)
            {
                int px1 = image.at<uchar>(y, x);

                for (size_t d = 0; d < numDirections; ++d)
                {
                    int nx = x + directions[d].first;
                    int ny = y + directions[d].second;

                    if (nx >= 0 && ny >= 0 && nx < cols && ny < rows)
                    {
                        int px2 = image.at<uchar>(ny, nx);
                        threadLocalGLCMs[d].at<float>(px1, px2)++;
                    }
                }
            }
        }

        #pragma omp for
        for (size_t i = 0; i < numDirections; ++i)
        {
            cv::Mat glcm = threadLocalGLCMs[i];
            double glcmSum = cv::sum(glcm)[0];
            glcm /= glcmSum; // Normalize the GLCM

            double localContrast = 0.0, localEnergy = 0.0;
            for (int j = 0; j < grayLevels; ++j)
            {
                for (int k = 0; k < grayLevels; ++k)
                {
                    double val = glcm.at<float>(j, k);
                    localContrast += (j - k) * (j - k) * val;
                    localEnergy += val * val;
                }
            }

            contrast[i] = localContrast;
            energy[i] = localEnergy;
        }
    }
}