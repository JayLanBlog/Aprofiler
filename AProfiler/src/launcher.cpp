#include <run/app/profile_app.h>

#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

using namespace APROFILER;

int main(int, char**) {
	ProfilerApp* app = new ProfilerApp();
	app->Run();
	//string path = "E:/WorkLearn/FrameWork/AProfiler/AProfiler/resource/res/apple.png";
	//Mat img = imread(path, IMREAD_COLOR);
	//// ���ͼ���Ƿ�ɹ�����
	//if (img.empty()) {
	//	std::cout << "�޷�����ͼ��" << std::endl;
	//	return -1;
	//}
	//Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;
	//imshow("Image", img);
	//waitKey(0);


	return 0;

}