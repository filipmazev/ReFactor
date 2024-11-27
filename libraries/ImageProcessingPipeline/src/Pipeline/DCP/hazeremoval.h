#ifndef HAZE_REMOVAL_H
#define HAZE_REMOVAL_H

#include "guidedfilter.h"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using std::cout;
using std::cerr;
using std::endl;

#pragma region Common Constants
const int CHANNEL_REQUIREMENT = 3;

const int radius = 7;
const double omega = 0.95;
const double t0 = 0.1;
const int r = 60;
const double eps = 0.001;
#pragma endregion 

typedef struct _pixel
{
	int i;
	int j;
	uchar val;
	_pixel(int _i, int _j, uchar _val) : i(_i), j(_j), val((uchar)_val) {}
} Pixel;

class CHazeRemoval
{
public:
	CHazeRemoval();
	~CHazeRemoval();

public:
	bool Init(int width, int height, int nChannels);
	bool Process(const unsigned char *indata, unsigned char *outdata, unsigned char *fogdata, cv::Vec3d &airlight, int width, int height, int nChannels);

private:
	int rows;
	int cols;
	int channels;
};

#pragma region Dark Channel Prior Calculations
void get_dark_channel(const cv::Mat *p_src, std::vector<Pixel> &tmp_vec, int rows, int cols, int channels, int radius);

void get_air_light(const cv::Mat *p_src, std::vector<Pixel> &tmp_vec, cv::Vec3d *p_Alight, int rows, int cols, int channels);

void get_transmission(const cv::Mat *p_src, cv::Mat *p_tran, cv::Vec3d *p_Alight, int rows, int cols, int channels, int radius, double omega);

void guided_filter(const cv::Mat *p_src, const cv::Mat *p_tran, cv::Mat *p_gtran, int r, double eps);

void get_fog(const cv::Mat *p_src, const cv::Mat *p_gtran, cv::Mat *p_fog, int rows, int cols, int channels);

void recover(const cv::Mat *p_src, const cv::Mat *p_gtran, cv::Mat *p_dst, cv::Vec3d *p_Alight, int rows, int cols, int channels, double t0);

void assign_data(unsigned char *outdata, const cv::Mat *p_dst, int rows, int cols, int channels);
#pragma endregion

#endif // !HAZE_REMOVAL_H