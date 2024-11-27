#include "glcm.h"

GLCMFeatures calc_GLCM_parallel(const cv::Mat &image, int grayLevels)
{
    std::vector<double> contrast, energy, homogenity, entropy;

    const int rows = image.rows;
    const int cols = image.cols;
    const size_t numDirections = directions.size();

    energy.resize(numDirections, 0.0);
    contrast.resize(numDirections, 0.0);
    homogenity.resize(numDirections, 0.0);
    entropy.resize(numDirections, 0.0);

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
            glcm /= glcmSum;

            double localContrast = 0.0, localEnergy = 0.0, localhomogenity = 0.0, localEntropy = 0.0;
            for (int j = 0; j < grayLevels; ++j)
            {
                for (int k = 0; k < grayLevels; ++k)
                {
                    double val = glcm.at<float>(j, k);
                    double diff = (j - k) * (j - k);
                    localContrast += diff * val;
                    localEnergy += val * val;

                    localhomogenity += val / (1 + std::abs(j - k));

                    if (val > 0)
                    {
                        localEntropy += val * std::log(val);
                    }
                }
            }

            energy[i] = localEnergy;
            contrast[i] = localContrast;
            homogenity[i] = localhomogenity;
            entropy[i] = -localEntropy;
        }
    }

    GLCMFeatures result;

    result.energy = std::accumulate(energy.begin(), energy.end(), 0.0) / numDirections;
    result.contrast = std::accumulate(contrast.begin(), contrast.end(), 0.0) / numDirections;
    result.homogenity = std::accumulate(homogenity.begin(), homogenity.end(), 0.0) / numDirections;
    result.entropy = std::accumulate(entropy.begin(), entropy.end(), 0.0) / numDirections;

    return result;
}