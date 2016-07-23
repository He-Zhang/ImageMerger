#include "stitch.h"

using namespace std;
using namespace cv;


static int GenFinger(Mat& Img, uLong * &finger)
{
	int height = Img.rows;
	int width = Img.cols;
	int ch = Img.channels();

	finger = new unsigned long[height];

	for (int nY = 0; nY<height; nY++)
	{
		uchar *p = Img.ptr<uchar>(nY);
		finger[nY] = crc32(0, p, width * ch);
	}

	return height;
}

Mat ImageGenerato(Mat& firstImg, Mat& secondImg)
{
	uLong* firstFinger;
	uLong* secondFinger;

	int firstLines = GenFinger(firstImg, firstFinger);
	int secondLines = GenFinger(secondImg, secondFinger);

	int **matrix = new int*[firstLines];
	for (int i = 0; i < firstLines; i++)
	{
		matrix[i] = new int[secondLines];
	}

	long long firstValueInSecondLines = secondFinger[0];
	for (int i = 0; i < firstLines; i++)
	{
		long long value = firstFinger[i];
		matrix[i][0] = value == firstValueInSecondLines;
	}

	long long firstValueInFirstLines = firstFinger[0];
	for (int i = 0; i < secondLines; i++)
	{
		long long value = secondFinger[i];
		matrix[0][i] = value == firstValueInFirstLines;
	}

	int length = 0;
	int x = 0;
	int y = 0;

	for (int i = 1; i < firstLines; i++)
	{
		for (int j = 1; j < secondLines; j++)
		{
			if (firstFinger[i] == secondFinger[j])
			{
				int value = matrix[i - 1][j - 1] + 1;
				matrix[i][j] = value;
				if (value > length)
				{
					length = value;
					x = i;
					y = j;
				}
			}
			else
			{
				matrix[i][j] = 0;
			}
		}
	}

	for (int i = 0; i < firstLines; ++i)
		delete[] matrix[i];
	delete[] matrix;

	delete[] firstFinger;
	delete[] secondFinger;

	Mat dst = Mat::zeros(x + secondImg.rows - y - 1, firstImg.cols, firstImg.type());

	Rect top(0, 0, firstImg.cols, x);
	firstImg.rowRange(0, x).copyTo(dst(top));

	Rect bottom(0, x, firstImg.cols, secondImg.rows - y - 1);
	secondImg.rowRange(y + 1, secondImg.rows).copyTo(dst(bottom));

	return dst;
}