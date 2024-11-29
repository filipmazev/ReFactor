#include "pch.h"
#include "hazeremoval.h"

CHazeRemoval::CHazeRemoval()
{
	rows = 0;
	cols = 0;
	channels = 0;
}

CHazeRemoval::~CHazeRemoval()
{
}

bool CHazeRemoval::Init(int width, int height, int nChannels)
{
	bool result = false;

	this->rows = height;
	this->cols = width;
	this->channels = nChannels;

	if (width > 0 && height > 0 && nChannels == CHANNEL_REQUIREMENT)
	{
		result = true;
	}

	return result;
}

bool CHazeRemoval::Process(const unsigned char* indata, unsigned char* outdata, unsigned char* fogdata, cv::Vec3d& airlight, int width, int height, int nChannels)
{
	bool result = true;
	if (!indata || !outdata || !fogdata)
	{
		result = false;
	}

	result = Init(width, height, nChannels);

	if (!result)
	{
		cerr << "Error initializing the Haze Removal process! Please provide valid values for width, height and nChannels" << endl;
		return result;
	}

	std::vector<Pixel> tmp_vec;

	Mat* p_src = new Mat(rows, cols, CV_8UC3, (void*)indata);
	Mat* p_dst = new Mat(rows, cols, CV_64FC3);
	Mat* p_tran = new Mat(rows, cols, CV_64FC1);
	Mat* p_gtran = new Mat(rows, cols, CV_64FC1);
	Mat* p_fog = new Mat(rows, cols, CV_64FC3);
	Vec3d* p_Alight = new Vec3d();

	get_dark_channel(p_src, tmp_vec, rows, cols, channels, radius);
	get_air_light(p_src, tmp_vec, p_Alight, rows, cols, channels);
	get_transmission(p_src, p_tran, p_Alight, rows, cols, channels, radius, omega);
	guided_filter(p_src, p_tran, p_gtran, r, eps);
	get_fog(p_src, p_gtran, p_fog, rows, cols, channels);

	airlight = *p_Alight;

	recover(p_src, p_gtran, p_dst, p_Alight, rows, cols, channels, t0);

	assign_data(outdata, p_dst, rows, cols, channels);
	assign_data(fogdata, p_fog, rows, cols, channels);

	return result;
}

#pragma region Dark Channel Prior Calculations
bool sort_asc(const Pixel& a, const Pixel& b)
{
	return a.val > b.val;
}

void get_dark_channel(const cv::Mat* p_src, std::vector<Pixel>& tmp_vec, int rows, int cols, int channels, int radius)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int rmin = cv::max(0, i - radius);
			int rmax = cv::min(i + radius, rows - 1);
			int cmin = cv::max(0, j - radius);
			int cmax = cv::min(j + radius, cols - 1);
			double min_val = 255;
			for (int x = rmin; x <= rmax; x++)
			{
				for (int y = cmin; y <= cmax; y++)
				{
					cv::Vec3b tmp = p_src->ptr<cv::Vec3b>(x)[y];
					uchar b = tmp[0];
					uchar g = tmp[1];
					uchar r = tmp[2];
					uchar minpixel = b > g ? ((g > r) ? r : g) : ((b > r) ? r : b);
					min_val = cv::min((double)minpixel, min_val);
				}
			}
			tmp_vec.push_back(Pixel(i, j, uchar(min_val)));
		}
	}
	std::sort(tmp_vec.begin(), tmp_vec.end(), sort_asc);
}

void get_air_light(const cv::Mat* p_src, std::vector<Pixel>& tmp_vec, cv::Vec3d* p_Alight, int rows, int cols, int channels)
{
	int num = int(rows * cols * 0.001);
	double A_sum[3] = {
		0,
	};
	std::vector<Pixel>::iterator it = tmp_vec.begin();
	for (int cnt = 0; cnt < num; cnt++)
	{
		cv::Vec3b tmp = p_src->ptr<cv::Vec3b>(it->i)[it->j];
		A_sum[0] += tmp[0];
		A_sum[1] += tmp[1];
		A_sum[2] += tmp[2];
		it++;
	}
	for (int i = 0; i < 3; i++)
	{
		(*p_Alight)[i] = A_sum[i] / num;
	}
}

void get_transmission(const cv::Mat* p_src, cv::Mat* p_tran, cv::Vec3d* p_Alight, int rows, int cols, int channels, int radius, double omega)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int rmin = cv::max(0, i - radius);
			int rmax = cv::min(i + radius, rows - 1);
			int cmin = cv::max(0, j - radius);
			int cmax = cv::min(j + radius, cols - 1);
			double min_val = 255.0;
			for (int x = rmin; x <= rmax; x++)
			{
				for (int y = cmin; y <= cmax; y++)
				{
					cv::Vec3b tmp = p_src->ptr<cv::Vec3b>(x)[y];
					double b = (double)tmp[0] / (*p_Alight)[0];
					double g = (double)tmp[1] / (*p_Alight)[1];
					double r = (double)tmp[2] / (*p_Alight)[2];
					double minpixel = b > g ? ((g > r) ? r : g) : ((b > r) ? r : b);
					min_val = cv::min(minpixel, min_val);
				}
			}
			p_tran->ptr<double>(i)[j] = 1 - omega * min_val;
		}
	}
}

void guided_filter(const cv::Mat* p_src, const cv::Mat* p_tran, cv::Mat* p_gtran, int r, double eps)
{
	*p_gtran = guidedFilter(*p_src, *p_tran, r, eps);
}

void recover(const cv::Mat* p_src, const cv::Mat* p_gtran, cv::Mat* p_dst, cv::Vec3d* p_Alight, int rows, int cols, int channels, double t0)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int c = 0; c < channels; c++)
			{
				double val = (double(p_src->ptr<cv::Vec3b>(i)[j][c]) - (*p_Alight)[c]) / cv::max(t0, p_gtran->ptr<double>(i)[j]) + (*p_Alight)[c];
				p_dst->ptr<cv::Vec3d>(i)[j][c] = cv::max(0.0, cv::min(255.0, val));
			}
		}
	}
}

void assign_data(unsigned char* outdata, const cv::Mat* p_dst, int rows, int cols, int channels)
{
	for (int i = 0; i < rows * cols * channels; i++)
	{
		*(outdata + i) = (unsigned char)(*((double*)(p_dst->data) + i));
	}
}

void get_fog(const cv::Mat* p_src, const cv::Mat* p_gtran, cv::Mat* p_fog, int rows, int cols, int channels)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int c = 0; c < channels; c++)
			{
				double fog_val = double(p_src->ptr<cv::Vec3b>(i)[j][c]) * (1.0 - p_gtran->ptr<double>(i)[j]);
				p_fog->ptr<cv::Vec3d>(i)[j][c] = fog_val;
			}
		}
	}
}
#pragma endregion