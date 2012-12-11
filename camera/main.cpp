//#include <opencv2/opencv.hpp>
//#include <math.h>
//#include <windows.h>
//#include <process.h> /* _beginthread, _endthread */
//#include <iostream>
//#include <assert.h>
//#include <vector>
//#include <hash_map>
//#include <utility>
//#include "functions.h"
//using namespace cv;
//int ca_count=0;
//void ThreadFunc1(PVOID param)
//{
//	while(1)
//	{
//		Sleep(1000);
//		std::cout<<"This was draw by thread l"<<std::endl;
//	}
//}
//void mergeImage(Mat &dist,Mat &src){
//	if(dist.rows==src.rows&&dist.cols==src.cols){
//		for(int row=0;row<dist.rows;++row){
//			uchar *s=src.ptr(row);
//			uchar *d=dist.ptr(row);
//			for (int col=0;col<dist.cols;++col)
//			{
//				if(*s!=0){
//					//*d=*s;
//					*(d+1)=*s;
//					//*d=*s;
//					//++d;
//				}
//				d+=3;
//				s++;
//			}
//		}
//	}
//}
////
//Mat rightEyeImage,leftEyeImage;
//Mat rightEyeResult,leftEyeResult;
//bool leftRunning=false;
//bool rightRunning=false;
//CRITICAL_SECTION cs_left;
//CRITICAL_SECTION cs_right;
//HANDLE hMutex = NULL;
//bool willLeftCamWriteBuffer=false;
//
//void ThreadLeftFrameHandler(PVOID param)
//{
//	VideoCapture cap(1);
//	if(!cap.isOpened()){
//		std::cout<<"left eye camera open failed";
//		//return ;
//	}
//	leftRunning=true;
//	Mat leftBuffer;
//	while(true)
//	{
//		EnterCriticalSection(&cs_left);
//		//cap>>leftEyeImage;
//		leftEyeImage=imread("c:\\3l.jpg");
//		//Mat ret=bee::calcAverageMatrix(leftEyeImage,19);
//		//bee::showDoubleMatrix(ret);
//		LeaveCriticalSection(&cs_left);
//		leftBuffer=leftEyeImage.clone();
//		mergeImage(leftBuffer,leftEyeResult);
//		imshow("left",leftBuffer);
//		if(waitKey(30) >= 0) break;
//	}
//	rightRunning=false;
//}
//
//void ThreadRightFrameHandler(PVOID param)
//{
//	rightRunning=true;
//	VideoCapture cap(0);
//	if(!cap.isOpened()){
//		std::cout<<"right eye camera open failed";
//		//return ;
//	}
//	Mat rightBuffer;
//	rightEyeImage=imread("c:\\3r.jpg");
//	//Mat ret=bee::calcAverageMatrix(rightEyeImage,19);
//	while(true)
//	{
//		//cap>>rightEyeImage;
//		rightBuffer=rightEyeImage.clone();
//		mergeImage(rightBuffer,rightEyeResult);
//		imshow("right",rightBuffer);
//		if(waitKey(30) >= 0) break;
//	}
//	rightRunning=false;
//}
//
//void ThreadDistanceEstimation(PVOID param){
//	Mat leftBuffer,rightBuffer;
//	bool initilizeResultMat=false;
//	while(true){
//		if(leftEyeImage.cols!=0&&leftRunning&&rightEyeImage.cols!=0&&rightRunning){
//			EnterCriticalSection(&cs_left);
//			cvtColor(leftEyeImage,leftBuffer,CV_BGR2YUV);
//			LeaveCriticalSection(&cs_left);
//			cvtColor(rightEyeImage,rightBuffer,CV_BGR2YUV);
//			if(initilizeResultMat==false){
//				initilizeResultMat=true;
//				EnterCriticalSection(&cs_left);
//				cvtColor(leftEyeImage,leftEyeResult,CV_BGR2GRAY);
//				LeaveCriticalSection(&cs_left);
//				cvtColor(rightEyeImage,rightEyeResult,CV_BGR2GRAY);
//			}
//			std::vector<std::pair<Point2i,Point2i>> correspond;
//			for (uint i=0;i<6;++i)
//			{
//				for (uint j=0;j<8;++j)
//				{
//					Point2i xx=bee::findCorrespondArea(leftBuffer,rightBuffer,i*10,j*10,10,10,10);
//					EnterCriticalSection(&cs_left);
//					//imshow("result",leftEyeImage);
//					LeaveCriticalSection(&cs_left);
//					//waitKey(0);
//					correspond.push_back(std::make_pair(Point2i(i*20,j*20),xx));
//				}
//
//			}
//			/*for (uint i=0;i<16;++i)
//			{
//				rectangle(rightEyeResult,Point(min_y,min_x),Point(min_y+cols,min_x+rows),Scalar(255,255,255));
//			}*/
//			std::cout<<"find one:"<<ca_count++<<std::endl;
//		}
//	}
//}
//void ThreadDistanceEstimation2(PVOID param){
//	Mat leftBuffer,rightBuffer;
//	bool enter=false;
//	while(true){
//		if(leftEyeImage.cols!=0&&leftRunning&&rightEyeImage.cols!=0&&rightRunning){
//			EnterCriticalSection(&cs_left);
//			cvtColor(leftEyeImage,leftBuffer,CV_BGR2YUV);
//			LeaveCriticalSection(&cs_left);
//			cvtColor(rightEyeImage,rightBuffer,CV_BGR2YUV);
//			if(enter==false){
//				enter=true;
//				EnterCriticalSection(&cs_left);
//				cvtColor(leftEyeImage,leftEyeResult,CV_BGR2GRAY);
//				LeaveCriticalSection(&cs_left);
//				cvtColor(rightEyeImage,rightEyeResult,CV_BGR2GRAY);
//			}
//			std::vector<std::pair<Point2i,Point2i>> correspond;
//			for (uint i=0;i<120;i+=10)
//			{
//				for (uint j=0;j<40;j+=10)
//				{
//					Point2i xx=bee::findCorrespondArea(rightBuffer,leftBuffer,i,j,10,10,10);
//					rectangle(rightEyeResult,Point(j,i),Point(j+10,i+10),Scalar(255,255,255));
//					rectangle(leftEyeResult,Point(xx.y,xx.x),Point(xx.y+10,xx.x+10),Scalar(255,255,255));
//					EnterCriticalSection(&cs_left);
//					//imshow("result",leftEyeImage);
//					LeaveCriticalSection(&cs_left);
//					//waitKey(0);
//					correspond.push_back(std::make_pair(Point2i(i*20,j*20),xx));
//				}
//
//			}
//			/*for (uint i=0;i<16;++i)
//			{
//				rectangle(rightEyeResult,Point(min_y,min_x),Point(min_y+cols,min_x+rows),Scalar(255,255,255));
//			}*/
//			
//			std::cout<<"find one:"<<ca_count++<<std::endl;
//		}
//	}
//}
//void ThreadPerformaceEstimation(PVOID param){
//
//}
//void ThreadEdgeDetector(PVOID param){
//	Mat leftBuffer,rightBuffer;
//	bool enter=false;
//	while(true){
//		if(leftEyeImage.cols!=0&&leftRunning&&rightEyeImage.cols!=0&&rightRunning){
//			EnterCriticalSection(&cs_left);
//			cvtColor(leftEyeImage,leftBuffer,CV_BGR2GRAY);
//			LeaveCriticalSection(&cs_left);
//			cvtColor(rightEyeImage,rightBuffer,CV_BGR2GRAY);
//			if(enter==false){
//				enter=true;
//				EnterCriticalSection(&cs_left);
//				cvtColor(leftEyeImage,leftEyeResult,CV_BGR2GRAY);
//				LeaveCriticalSection(&cs_left);
//				cvtColor(rightEyeImage,rightEyeResult,CV_BGR2GRAY);
//			}
//			Mat leftEdge,rightEdge;
//			cv::Canny(leftBuffer,leftEdge,100,10);
//			cv::Canny(rightBuffer,rightEdge,100,10); 
//			imshow("edge",leftEdge);
//			imshow("rightEdge",rightEdge);
//			waitKey(0);
//			std::cout<<"find one:"<<ca_count++<<std::endl;
//		}
//	}
//}
///************************************************************************/
///*                                                                      */
///************************************************************************/
//int main1(){
//	InitializeCriticalSection(&cs_left);
//	InitializeCriticalSection(&cs_right);
//	hMutex = CreateMutex(NULL, TRUE, NULL);
//	_beginthread(ThreadLeftFrameHandler,0,NULL);
//	_beginthread(ThreadRightFrameHandler,0,NULL);
//
//	//_beginthread(ThreadDistanceEstimation,0,NULL);
//	_beginthread(ThreadDistanceEstimation2,0,NULL);
//	_beginthread(ThreadEdgeDetector,0,NULL);
//
//	_beginthread(ThreadPerformaceEstimation,0,NULL);
//	Sleep(11111111110);
//	/*Sleep(30000);
//	Mat m1=Mat::ones(10,5,CV_8UC3);
//	Mat m2=Mat::ones(10,5,CV_8UC3);
//	m2=m2*2;
//	Mat i3r=imread("c:\\3r.jpg");
//	Mat i3l=imread("C:\\3l.jpg");
//	Mat yuv3r,yuv3l;
//	cvtColor(i3r,yuv3r,CV_RGB2YUV);
//	cvtColor(i3l,yuv3l,CV_RGB2YUV);
//	imshow("yuv3l",yuv3l);
//	waitKey(0);
//	double d=matDistance(i3r,i3l,0,0,0,0,5,5);
//	int x_=findSimilarArea(yuv3l,yuv3r,196,75,40,40);
//	Mat gray1,gray2, dst1,dst2,edge; 
//	pyrMeanShiftFiltering(i3r,dst1,20,40,2);
//	pyrMeanShiftFiltering(i3l,dst2,20,40,2);
//
//	cvtColor(dst1,gray1,CV_RGB2GRAY);
//	imshow("a",gray1);
//	cvtColor(dst2,gray2,CV_RGB2GRAY);
//	imshow("b",gray2);
//	waitKey(0);
//	Sobel(gray2,edge,CV_16S,1,1,3);
//	imshow("a",edge);
//	waitKey(0);*/
//	return 0;
//}