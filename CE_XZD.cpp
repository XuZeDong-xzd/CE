#include "opencv2/core.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <io.h>
#include <opencv2/core/utils/logger.hpp>

using namespace cv;
using namespace std;

#define WIDTH 640 //指定图像的宽度，即cols
#define HEIGHT 512 //指定图像的高度，即rows

/**
* @brief 读取校正后的红外图像并存储为CV::MAT(CV_16UC1)
* @param[in] file_path 图像文件的路径
* @return image CV_16UC1格式图像
*/
Mat readImageFromRaw(std::string file_path) {
	FILE* fpsrc = NULL;  //读入文件指针
	uint16_t* src = new uint16_t[WIDTH * HEIGHT];  //读入

	const char* filenamechar = file_path.c_str();
	if ((fpsrc = fopen(filenamechar, "rb")) == NULL)
	{
		printf("can not open the raw image");
		exit;
	}
	else {
		printf("IMAGE read OK\n");
	}

	fread(src, sizeof(uint16_t), WIDTH * HEIGHT, fpsrc);

	//construct opencv mat and show image Size()先列后行,即先宽后高
	cv::Mat image(cv::Size(WIDTH, HEIGHT), CV_16UC1, src);
	fclose(fpsrc);

	return image;
}

Mat readImageFromY16(std::string file_path) {
	// 注意，这里要指定binary读取模式
	std::ifstream fin;
	fin.open(file_path, std::ios::binary);
	if (!fin) {
		std::cerr << "open failed: " << file_path << std::endl;
	}

	// load buffer
	char* buffer = new char[WIDTH * HEIGHT * 2];
	// read函数读取（拷贝）流中的length个字节到buffer
	fin.read(buffer, WIDTH * HEIGHT * 2);

	// construct opencv mat and show image Size()先列后行,即先宽后高
	cv::Mat image(cv::Size(WIDTH, HEIGHT), CV_16UC1);

	uint16_t* p_img;

	//先生成空的目标图片,先行后列，即先高后宽
	//Mat dst = Mat::zeros(image.rows, image.cols, CV_16UC1);

	for (int i = 0; i < image.rows; i++)
	{
		p_img = image.ptr<uint16_t>(i);//获取每行首地址
		for (int j = 0; j < image.cols; ++j)
		{
			//交换大小端模式
			unsigned char a[2] = { buffer[(i * image.cols + j) * 2 + 1], buffer[(i * image.cols + j) * 2] };
			//指针强制转换
			short result = *((short*)a);

			p_img[j] = result + 8192;
			if (p_img[j] <= 0)
				p_img[j] = 0;
			if (p_img[j] > 16383)
				p_img[j] = 16383;
		}
	}

	//flip(image, image, -1); //旋转180度
	return image;
}


/**
* @brief 将16位图像压缩位8位图像
* @param[in] src 16位ushort格式图像
* @param[in] dst 8位uchar格式图像
*/
void imageCompress(Mat& src, Mat& dst) {
	//先生成空的目标图片,先行后列，即先高后宽
	dst = Mat::zeros(src.rows, src.cols, CV_8UC1);
	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;
	//取得像素值最大值和最小值
	minMaxIdx(src, minp, maxp);

	//对16位格式的数据进行压缩,用指针访问像素，速度更快
	ushort* p_img;
	uchar* p_dst;
	for (int i = 0; i < src.rows; i++)
	{
		p_img = src.ptr<ushort>(i);//获取每行首地址
		p_dst = dst.ptr<uchar>(i);
		for (int j = 0; j < src.cols; ++j)
		{
			p_dst[j] = (p_img[j] - minv) / (maxv - minv) * 255;
		}
	}
}




//2009
void Xuzedong_CE(const cv::Mat& src, cv::Mat& dst, int threshold = 5, double alpha = 20, int g = 10)
{
	暂不开源
}




int main()
{
	cv::utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT);
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);

	Mat image = imread("D:/infraredProject/dataset/INFRARED100/75.png", 0);
	//Mat imageRaw = imread("C:/Users/24563/Desktop/infraredProject/dataset/marathon-test-seq1-raw/raw/frame_00001.png", CV_16UC1);
	/*Mat imageRaw = readImageFromY16("../dataset/true/jianzhu.raw");
	Mat image;
	imageCompress(imageRaw, image);*/

	imshow("image_gray", image);
	

	Mat Xuzedong_CE_img(cv::Size(WIDTH, HEIGHT), CV_8UC1);
	Xuzedong_CE(image, Xuzedong_CE_img);
	imshow("Xuzedong_CE_img", Xuzedong_CE_img);


	waitKey(0);
	return 0;
}





