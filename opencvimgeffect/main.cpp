#include<iostream>
#include<opencv2/opencv.hpp>
#include<future>

using namespace cv;
using namespace std;

Mat src, dst, tmpsrc;
//方向显示
void directionshow(Mat, int, int, int, bool windowflag = false);
//图像渐显
void graduallyshow(Mat, int, int);

//马赛克显示
void mosaicshow(Mat, int, int, int);

//平移显示
void translationshow(Mat, int, int, int);


//窗口显示
void setshowwindow(Mat ,string, int, int);

//中间扩张
void midexpandshow(Mat src, int width, int height, int direction = 0);

//栅条特效
void gridshow(Mat src, int width, int height, int gridwidth, int direction = 0);

int main(int argc, char** argv) {
	src = imread("E:/DCIM/test3.jpg");
	if (!src.data) {
		cout << "could not read src" << endl;
		return -1;
	}
	
	imshow("src", src);

	//展开显示
	//future<void> vertical0 = async(launch::async, directionshow, src, src.cols, src.rows, 0, true);
	//future<void> vertical1 = async(launch::async, directionshow, src, src.cols, src.rows, 1, true);
	//future<void> vertical2 = async(launch::async, directionshow, src, src.cols, src.rows, 2, true);
	//future<void> vertical3 = async(launch::async, directionshow, src, src.cols, src.rows, 3, true);
	
	//图像渐显
	//future<void> ftgradually = async(launch::async, graduallyshow, src, src.cols, src.rows);

	//马赛克显示
	//future<void> ftmosaic = async(launch::async, mosaicshow, src, src.cols, src.rows, 20);

	//平移显示
	//future<void> fttranslation0 = async(launch::async, translationshow, src, src.cols, src.rows, 0);
	//future<void> fttranslation1 = async(launch::async, translationshow, src, src.cols, src.rows, 1);
	//future<void> fttranslation2 = async(launch::async, translationshow, src, src.cols, src.rows, 2);
	//future<void> fttranslation3 = async(launch::async, translationshow, src, src.cols, src.rows, 3);

	//中间扩张显示
	//future<void> ftmidexpand0 = async(launch::async, midexpandshow, src, src.cols, src.rows, 0);
	//future<void> ftmidexpand1 = async(launch::async, midexpandshow, src, src.cols, src.rows, 1);

	//栅格显示
	future<void> ftgridshow0 = async(launch::async, gridshow, src, src.cols, src.rows, 18, 0);
	future<void> ftgridshow1 = async(launch::async, gridshow, src, src.cols, src.rows, 18, 1);


	waitKey(0);
	return 0;
}

//垂直方向显示   direction 0-从上到下  1-从下到上 2-从左向右  3-从右向左
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

//图像渐显  
//参数：Mat 源图像，
//      width图像宽度, 
//      height图像高度
//思路：1.记录下图像每个像素点的像素值，显示的时候先将屏幕置黑
//      2.循环显示图像N次，每次显示像素值为N/256倍
void graduallyshow(Mat src, int width, int height)
{
	Mat dst;
	dst = src.clone();
	//考虑到三通道RGB的问题，所以宽度这里要乘3
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

//马赛克显示   
//参数：src 源图像， 
//      width 源图像宽度， 
//      height 源图像高度,
//      sidelength 马赛克的边长
//思路：1.根据设置的边长定义图像中马赛克的正方形小块，
//        把每个小块的左上角坐标记录下来存放到vector的容器中
//      2.然后把vector的容器顺序打乱
//      3.遍历容器里的坐标显示出图像
void mosaicshow(Mat src, int width, int height, int sidelength)
{
	Mat dst = Mat(src.size(), CV_8UC3);
	//定义坐标点容器
	vector<Point2f> vtpoints;

	//遍历图像将正方形小块左上角点存到容器中
	for (int col = 0; col < width; col = col + sidelength) {
		for (int row = 0; row < height; row = row + sidelength) {
			//如果宽度超出图像最右边的正方形点后设置最右边点
			if (col > width - sidelength) {
				col = width - sidelength;
			}
			//如果高度超出图像最下面的正方形高度同上原理
			if (row > height - sidelength) {
				row = height - sidelength;
			}
			//将坐标点存入到容器中
			vtpoints.push_back(Point2f(col, row));
		}
	}

	//打乱容器内的坐标点
	random_shuffle(vtpoints.begin(), vtpoints.end());

	//遍历容器显示每个马赛克
	for (Point2f pt : vtpoints)
	{
		src(Rect(pt.x, pt.y, sidelength, sidelength))
			.copyTo(dst(Rect(pt.x, pt.y, sidelength, sidelength)));
		imshow("mosaicshow", dst);
		waitKey(1);
	}
	waitKey(0);
}

//平移显示
//参数：Mat 源图像，
//      width图像宽度, 
//      height图像高度
//      direction方向 0-从上到下  1-从下到上  2-从左到右  3-从右到左
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

//显示窗口设置  
//参数  img 显示的图像源，
//      winname 显示的窗口名称,
//      pointx  显示的坐标x
//      pointy  显示的坐标y
void setshowwindow(Mat img, string winname, int pointx, int pointy)
{
	//设置显示窗口
	namedWindow(winname, WindowFlags::WINDOW_NORMAL);
	//设置图像显示大小
	resizeWindow(winname, img.size());
	//设置图像显示位置
	moveWindow(winname, pointx, pointy);
}

//中间扩张显示
//参数：  src 源图像,
//       width 图像宽度
//       height 图像长度
//       direction  方向  0-上下扩张  1-左右扩张
void midexpandshow(Mat src, int width, int height, int direction)
{
	Mat dst = Mat(src.size(), CV_8UC3);
	if (direction == 0) {
		setshowwindow(src, "midexpandshow0", 1, 100);
		//上下方向扩张
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
		//左右方向扩张
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


//栅条特效
//参数   src  图像源
//       width  图像宽度
//       height 图像高度
//       gridwidth 每个栅格宽度
//       direction 方向   0-水平方向   1-垂直方向
void gridshow(Mat src, int width, int height, int gridwidth, int direction)
{
	Mat dst = Mat(src.size(), CV_8UC3);

	if (direction == 0) {
		setshowwindow(src, "gridshow0", src.cols * 2 + 1, 100);
		for (int i = 1; i < width + 1; i = i + gridwidth) {
			//当I等于源图像宽度时，说明栅条全部遍历完成，直接显示原图像
			if (i == width) {
				cout << "over" << endl;
				src.copyTo(dst);
				imshow("gridshow0", dst);
				break;
			}
			if (i > width - gridwidth) i = width - gridwidth;
			for (int j = 0; j < height; j = j + 2 * gridwidth) {
				if (j > height - 2 * gridwidth) j = height - 2 * gridwidth;
				//奇数行从右往左
				src(Rect(0, j, i, gridwidth))
					.copyTo(dst(Rect(width - i, j, i, gridwidth)));

				//偶数行从左往右
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
			//当I等于源图像宽度时，说明栅条全部遍历完成，直接显示原图像
			if (i == height) {
				cout << "over" << endl;
				src.copyTo(dst);
				imshow("gridshow1", dst);
				break;
			}
			if (i > height - gridwidth) i = height - gridwidth;
			for (int j = 0; j < width; j = j + 2 * gridwidth) {
				if (j > width - 2 * gridwidth) j = width - 2 * gridwidth;
				//奇数列从上往下
				src(Rect(j, height - i, gridwidth, i))
					.copyTo(dst(Rect(j, 0, gridwidth, i)));

				//偶数列从下往上
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


