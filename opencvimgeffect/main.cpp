#include<iostream>
#include<opencv2/opencv.hpp>
#include<future>

using namespace cv;
using namespace std;

Mat src, dst, tmpsrc;
//������ʾ
void directionshow(Mat, int, int, int, bool windowflag = false);
//ͼ����
void graduallyshow(Mat, int, int);

//��������ʾ
void mosaicshow(Mat, int, int, int);

//ƽ����ʾ
void translationshow(Mat, int, int, int);


//������ʾ
void setshowwindow(Mat ,string, int, int);

//�м�����
void midexpandshow(Mat src, int width, int height, int direction = 0);

//դ����Ч
void gridshow(Mat src, int width, int height, int gridwidth, int direction = 0);

int main(int argc, char** argv) {
	src = imread("E:/DCIM/test3.jpg");
	if (!src.data) {
		cout << "could not read src" << endl;
		return -1;
	}
	
	imshow("src", src);

	//չ����ʾ
	//future<void> vertical0 = async(launch::async, directionshow, src, src.cols, src.rows, 0, true);
	//future<void> vertical1 = async(launch::async, directionshow, src, src.cols, src.rows, 1, true);
	//future<void> vertical2 = async(launch::async, directionshow, src, src.cols, src.rows, 2, true);
	//future<void> vertical3 = async(launch::async, directionshow, src, src.cols, src.rows, 3, true);
	
	//ͼ����
	//future<void> ftgradually = async(launch::async, graduallyshow, src, src.cols, src.rows);

	//��������ʾ
	//future<void> ftmosaic = async(launch::async, mosaicshow, src, src.cols, src.rows, 20);

	//ƽ����ʾ
	//future<void> fttranslation0 = async(launch::async, translationshow, src, src.cols, src.rows, 0);
	//future<void> fttranslation1 = async(launch::async, translationshow, src, src.cols, src.rows, 1);
	//future<void> fttranslation2 = async(launch::async, translationshow, src, src.cols, src.rows, 2);
	//future<void> fttranslation3 = async(launch::async, translationshow, src, src.cols, src.rows, 3);

	//�м�������ʾ
	//future<void> ftmidexpand0 = async(launch::async, midexpandshow, src, src.cols, src.rows, 0);
	//future<void> ftmidexpand1 = async(launch::async, midexpandshow, src, src.cols, src.rows, 1);

	//դ����ʾ
	future<void> ftgridshow0 = async(launch::async, gridshow, src, src.cols, src.rows, 18, 0);
	future<void> ftgridshow1 = async(launch::async, gridshow, src, src.cols, src.rows, 18, 1);


	waitKey(0);
	return 0;
}

//��ֱ������ʾ   direction 0-���ϵ���  1-���µ��� 2-��������  3-��������
void directionshow(Mat src, int width, int height, int direction, bool windowflag) {
	Mat tmpsrc, dst;
	if (direction == 0) {
		if(windowflag) setshowwindow(src, "direction0", 1, 100);
		for (int i = 1; i < height; i++) {
			tmpsrc = src(Rect(0, 0, width, i));
			tmpsrc.copyTo(dst);

			imshow("direction0", dst);
			waitKey(1);
		}
	}
	else if (direction == 1) {
		if (windowflag) setshowwindow(src, "direction1", src.cols + 1, 100);
		for (int i = 1; i < height; ++i) {
			tmpsrc = src(Rect(0, height- i, width, i));
			tmpsrc.copyTo(dst);

			imshow("direction1", dst);
			waitKey(1);
		}
	}
	else if (direction == 2) {
		if (windowflag) setshowwindow(src, "direction2", src.cols * 2 + 1, 100);
		for (int i = 1; i < width; ++i) {
			tmpsrc = src(Rect(0, 0, i, height));
			tmpsrc.copyTo(dst);

			imshow("direction2", dst);
			waitKey(1);
		}
	}
	else {
		if (windowflag) setshowwindow(src, "direction3", src.cols * 3 + 1, 100);
		for (int i = width - 1; i > 1; --i) {
			tmpsrc = src(Rect(i, 0, width - i, height));
			tmpsrc.copyTo(dst);

			imshow("direction3", dst);
			waitKey(1);
		}
	}
	cout << "over" << direction << endl;
	waitKey(0);
}

//ͼ����  
//������Mat Դͼ��
//      widthͼ����, 
//      heightͼ��߶�
//˼·��1.��¼��ͼ��ÿ�����ص������ֵ����ʾ��ʱ���Ƚ���Ļ�ú�
//      2.ѭ����ʾͼ��N�Σ�ÿ����ʾ����ֵΪN/256��
void graduallyshow(Mat src, int width, int height)
{
	Mat dst;
	dst = src.clone();
	//���ǵ���ͨ��RGB�����⣬���Կ������Ҫ��3
	int tmpwidth = width * 3;
	for (int n = 1; n < 256; ++n) {
		for (int row = 0; row < height; ++row) {
			uchar* data1 = src.ptr<uchar>(row);
			uchar* data2 = dst.ptr<uchar>(row);

			for (int col = 0; col < tmpwidth; ++col) {
				data2[col] = data1[col] * n / 256;
			}
		}
		imshow("graduallyshow", dst);
		waitKey(1);
	}
	cout << "graduallyshow" << endl;
	waitKey(0);
}

//��������ʾ   
//������src Դͼ�� 
//      width Դͼ���ȣ� 
//      height Դͼ��߶�,
//      sidelength �����˵ı߳�
//˼·��1.�������õı߳�����ͼ���������˵�������С�飬
//        ��ÿ��С������Ͻ������¼������ŵ�vector��������
//      2.Ȼ���vector������˳�����
//      3.�����������������ʾ��ͼ��
void mosaicshow(Mat src, int width, int height, int sidelength)
{
	Mat dst = Mat(src.size(), CV_8UC3);
	//�������������
	vector<Point2f> vtpoints;

	//����ͼ��������С�����Ͻǵ�浽������
	for (int col = 0; col < width; col = col + sidelength) {
		for (int row = 0; row < height; row = row + sidelength) {
			//�����ȳ���ͼ�����ұߵ������ε���������ұߵ�
			if (col > width - sidelength) {
				col = width - sidelength;
			}
			//����߶ȳ���ͼ��������������θ߶�ͬ��ԭ��
			if (row > height - sidelength) {
				row = height - sidelength;
			}
			//���������뵽������
			vtpoints.push_back(Point2f(col, row));
		}
	}

	//���������ڵ������
	random_shuffle(vtpoints.begin(), vtpoints.end());

	//����������ʾÿ��������
	for (Point2f pt : vtpoints)
	{
		src(Rect(pt.x, pt.y, sidelength, sidelength))
			.copyTo(dst(Rect(pt.x, pt.y, sidelength, sidelength)));
		imshow("mosaicshow", dst);
		waitKey(1);
	}
	waitKey(0);
}

//ƽ����ʾ
//������Mat Դͼ��
//      widthͼ����, 
//      heightͼ��߶�
//      direction���� 0-���ϵ���  1-���µ���  2-������  3-���ҵ���
void translationshow(Mat src, int width, int height, int direction)
{
	Mat dst = Mat(src.size(), CV_8UC3);
	if (direction == 0) {
		setshowwindow(src, "translationshow0", 1, 100);
		for (int i = 1; i < height; ++i) {
			src(Rect(0, height - i, width, i))
				.copyTo(dst(Rect(0, 0, width, i)));
			
			imshow("translationshow0", dst);
			waitKey(1);
		}
	}
	else if (direction == 1) {
		setshowwindow(src, "translationshow1", src.cols + 1, 100);
		for (int i = 1; i < height; ++i) {
			src(Rect(0, 0, width, i))
				.copyTo(dst(Rect(0, height - i, width, i)));

			imshow("translationshow1", dst);
			waitKey(1);
		}
	}
	else if (direction == 2) {
		setshowwindow(src, "translationshow2", src.cols * 2 + 1, 100);
		for (int i = 1; i < width; ++i) {
			src(Rect(width - i, 0, i, height))
				.copyTo(dst(Rect(0, 0, i, height)));

			imshow("translationshow2", dst);
			waitKey(1);
		}
	}
	else if (direction == 3) {
		setshowwindow(src, "translationshow3", src.cols * 3 + 1, 100);
		for (int i = 1; i < width; ++i) {
			src(Rect(0, 0, i, height))
				.copyTo(dst(Rect(width - i, 0, i, height)));

			imshow("translationshow3", dst);
			waitKey(1);
		}
	}
	waitKey(0); 
}

//��ʾ��������  
//����  img ��ʾ��ͼ��Դ��
//      winname ��ʾ�Ĵ�������,
//      pointx  ��ʾ������x
//      pointy  ��ʾ������y
void setshowwindow(Mat img, string winname, int pointx, int pointy)
{
	//������ʾ����
	namedWindow(winname, WindowFlags::WINDOW_NORMAL);
	//����ͼ����ʾ��С
	resizeWindow(winname, img.size());
	//����ͼ����ʾλ��
	moveWindow(winname, pointx, pointy);
}

//�м�������ʾ
//������  src Դͼ��,
//       width ͼ����
//       height ͼ�񳤶�
//       direction  ����  0-��������  1-��������
void midexpandshow(Mat src, int width, int height, int direction)
{
	Mat dst = Mat(src.size(), CV_8UC3);
	if (direction == 0) {
		setshowwindow(src, "midexpandshow0", 1, 100);
		//���·�������
		for (int i = 1; i < height / 2; ++i) {
			src(Rect(0, height / 2 - i, width, i))
				.copyTo(dst(Rect(0, height / 2 - i, width, i)));
			src(Rect(0, height / 2, width, i))
				.copyTo(dst(Rect(0, height / 2, width, i)));

			imshow("midexpandshow0", dst);
			waitKey(1);
		}
	}
	else {
		setshowwindow(src, "midexpandshow1", src.cols + 1, 100);
		//���ҷ�������
		for (int i = 1; i < width / 2; ++i) {
			src(Rect(width / 2 - i, 0, i, height))
				.copyTo(dst(Rect(width / 2 - i, 0, i, height)));
			src(Rect(width / 2, 0, i, height))
				.copyTo(dst(Rect(width / 2, 0, i, height)));

			imshow("midexpandshow1", dst);
			waitKey(1);
		}
	}
	waitKey(0);
}


//դ����Ч
//����   src  ͼ��Դ
//       width  ͼ����
//       height ͼ��߶�
//       gridwidth ÿ��դ����
//       direction ����   0-ˮƽ����   1-��ֱ����
void gridshow(Mat src, int width, int height, int gridwidth, int direction)
{
	Mat dst = Mat(src.size(), CV_8UC3);

	if (direction == 0) {
		setshowwindow(src, "gridshow0", src.cols * 2 + 1, 100);
		for (int i = 1; i < width + 1; i = i + gridwidth) {
			//��I����Դͼ����ʱ��˵��դ��ȫ��������ɣ�ֱ����ʾԭͼ��
			if (i == width) {
				cout << "over" << endl;
				src.copyTo(dst);
				imshow("gridshow0", dst);
				break;
			}
			if (i > width - gridwidth) i = width - gridwidth;
			for (int j = 0; j < height; j = j + 2 * gridwidth) {
				if (j > height - 2 * gridwidth) j = height - 2 * gridwidth;
				//�����д�������
				src(Rect(0, j, i, gridwidth))
					.copyTo(dst(Rect(width - i, j, i, gridwidth)));

				//ż���д�������
				int k = j + gridwidth;

				src(Rect(width - i, k, i, gridwidth))
					.copyTo(dst(Rect(0, k, i, gridwidth)));

				imshow("gridshow0", dst);
				waitKey(1);
			}
		}
	}
	else {
		setshowwindow(src, "gridshow1", src.cols * 3 + 1, 100);
		for (int i = 1; i < height + 1; i = i + gridwidth) {
			//��I����Դͼ����ʱ��˵��դ��ȫ��������ɣ�ֱ����ʾԭͼ��
			if (i == height) {
				cout << "over" << endl;
				src.copyTo(dst);
				imshow("gridshow1", dst);
				break;
			}
			if (i > height - gridwidth) i = height - gridwidth;
			for (int j = 0; j < width; j = j + 2 * gridwidth) {
				if (j > width - 2 * gridwidth) j = width - 2 * gridwidth;
				//�����д�������
				src(Rect(j, height - i, gridwidth, i))
					.copyTo(dst(Rect(j, 0, gridwidth, i)));

				//ż���д�������
				int k = j + gridwidth;

				src(Rect(k, 0, gridwidth, i))
					.copyTo(dst(Rect(k, height - i, gridwidth, i)));

				imshow("gridshow1", dst);
				waitKey(1);
			}
		}
	}
	waitKey(0);
}


