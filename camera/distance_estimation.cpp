#include <opencv2/opencv.hpp>
#include <math.h>
#include <windows.h>
#include <process.h> /* _beginthread, _endthread */
#include <iostream>
#include <assert.h>
#include <vector>
#include <hash_map>
#include <utility>
#include "common.h"
using namespace std;
using namespace cv;
const int patchSize=20;
typedef   HANDLE   Semaphore;   //   信号量的Windows原型   
#define   P(S)   WaitForSingleObject(S,   INFINITE)   //   定义Windows下的P操作
#define   V(S)   ReleaseSemaphore(S,   1,   NULL)     //   定义Windows下的V操作

Mat rightEyeImg,leftEyeImg,rightEyeResult,leftEyeResult,leftEdgeImg,rightEdgeImg;

CRITICAL_SECTION cs_leftImg,cs_rightImg,cs_leftEdgeImg,cs_rightEdgeImg;
CRITICAL_SECTION cs_leftImgResult,cs_rightImgResult ;
vector<pair<Point2i,Point2i>> patchPairs;
Semaphore patchPairsReady;
int ca_count=0;
bool leftRunning=false,rightRunning=false;

void onMouse( int event, int x, int y, int, void* ){
	if( event != CV_EVENT_LBUTTONDOWN ){
		return;
	}
	char num[20];
	itoa(leftEdgeImg.at<uchar>(y,x),num,10);
	cout<<x<<"\t"<<y<<endl;
	cout<<num<<endl;
}
void ThreadEdgeDetector(PVOID param){

}
void ThreadEstimateDistance(PVOID param){
	while (true)
	{
		P(patchPairsReady);
		//cout<<"distance estimate"<<endl;
		matSetZero(leftEyeResult);
		matSetZero(rightEyeResult);
		for (uint i=0;i<patchPairs.size();++i)
		{
			pair<Point2i,Point2i> tmpPointPair=patchPairs[i];
			int distance=abs(tmpPointPair.first.y-tmpPointPair.second.y);
			rectangle(leftEyeResult,Point(tmpPointPair.first.y,tmpPointPair.first.x),Point(tmpPointPair.first.y+patchSize-1,tmpPointPair.first.x+patchSize-1),Scalar(distance*10));
			rectangle(rightEyeResult,Point(tmpPointPair.second.y,tmpPointPair.second.x),Point(tmpPointPair.second.y+patchSize-1,tmpPointPair.second.x+patchSize-1),Scalar(distance*10));
		}
	}
}
void ThreadSearchCorrespondence(PVOID param){
	Mat leftBuffer,rightBuffer;

	bool initilizeResultMat=false;
	while(true){
		if(leftEyeImg.cols!=0&&leftRunning&&rightEyeImg.cols!=0&&rightRunning){
			//获取摄像机捕获图像作为缓存
			EnterCriticalSection(&cs_leftImg);
			cvtColor(leftEyeImg,leftBuffer,CV_BGR2YUV);
			LeaveCriticalSection(&cs_leftImg);
			EnterCriticalSection(&cs_rightImg);
			cvtColor(rightEyeImg,rightBuffer,CV_BGR2YUV);
			LeaveCriticalSection(&cs_rightImg);
			if(initilizeResultMat==false){
				initilizeResultMat=true;
				//初始化结果图像
				EnterCriticalSection(&cs_leftImg);
				cvtColor(leftEyeImg,leftEyeResult,CV_BGR2GRAY);
				LeaveCriticalSection(&cs_leftImg);
				EnterCriticalSection(&cs_rightImg);
				cvtColor(rightEyeImg,rightEyeResult,CV_BGR2GRAY);
				LeaveCriticalSection(&cs_rightImg);
			}
			Mat leftEdgeBuffer, rightEdgeBuffer;
			EnterCriticalSection(&cs_leftEdgeImg);
			leftEdgeBuffer=leftEdgeImg;
			LeaveCriticalSection(&cs_leftEdgeImg);
			EnterCriticalSection(&cs_rightEdgeImg);
			rightEdgeBuffer=rightEdgeImg;
			LeaveCriticalSection(&cs_rightEdgeImg);
			if(rightEdgeBuffer.cols==0||leftEdgeBuffer.cols==0){
				continue;
			}
			std::vector<std::pair<Point2i,Point2i>> correspond;
			double minDistance=0;
			double distanceAvg=0;
			double minDistanceThreshold=40;
			double distanceChangeThreshold=20;//在搜索空间里变化范围大小
			//左图的右半边向左到右边的图片中寻找
			for (int i=0;i<leftBuffer.rows-9;i+=patchSize)
			{
				for (int j=80;j<leftBuffer.cols;j+=patchSize)
				{
					if(hasEdgeInPatch(leftEdgeBuffer,Point2i(i,j),patchSize)){
						Point2i xx=getCorrespondPatchInRightImg(leftBuffer,rightBuffer,Point2i(i,j),minDistance,distanceAvg);
						if(minDistance<minDistanceThreshold){
							if(distanceAvg-minDistance>distanceChangeThreshold){//
								correspond.push_back(std::make_pair(Point2i(i,j),xx));
							}
						}else{

						}
					}
				}

			}
			//右图的左半边向右到左图中寻找

			for (int i=0;i<rightBuffer.rows-9;i+=patchSize)
			{
				for (int j=0;j<80;j+=patchSize)
				{
					if (hasEdgeInPatch(rightEdgeBuffer,Point2i(i,j),patchSize))
					{
						Point2i xx=getCorrespondPatchInLeftImg(rightBuffer,leftBuffer,Point2i(i,j),minDistance,distanceAvg);
						if(minDistance<minDistanceThreshold){
							if(distanceAvg-minDistance>distanceChangeThreshold){//
								correspond.push_back(std::make_pair(xx,Point2i(i,j)));
							}
						}else{

						}
					}
				}
			}
			patchPairs=correspond;
			correspond.clear();
			V(patchPairsReady);
			//std::cout<<"find one:"<<ca_count++<<std::endl;
		}
	}
}
void ThreadCaptureLeftImageAndShow(PVOID param){
	VideoCapture cap(1);
	if(!cap.isOpened()){
		std::cout<<"left eye camera open failed";
		return ;
	}
	Mat leftBuffer;
	Mat leftCaptureBuffer;
	Mat leftCaptureBuffer2;
	Mat leftEdgeBuffer1;
	Mat leftEdgeBuffer2;
	leftRunning=true;
	while(true)
	{
		cap>>leftCaptureBuffer2;

		//pyrDown(leftCaptureBuffer,leftCaptureBuffer2);
		EnterCriticalSection(&cs_leftImg);
		pyrDown(leftCaptureBuffer2,leftEyeImg);
		leftBuffer=leftEyeImg.clone();
		LeaveCriticalSection(&cs_leftImg);

		Canny(leftCaptureBuffer2,leftEdgeBuffer2,150,50);
		//pyrDown(leftEdgeBuffer1,leftEdgeBuffer2);

		EnterCriticalSection(&cs_leftEdgeImg);
		pyrDown(leftEdgeBuffer2,leftEdgeImg);
		LeaveCriticalSection(&cs_leftEdgeImg);

		EnterCriticalSection(&cs_leftImgResult);
		mergeImage(leftBuffer,leftEyeResult);
		LeaveCriticalSection(&cs_leftImgResult);

		imshow("left",leftBuffer);
		imshow("leftEdge",leftEdgeImg);
		setMouseCallback( "leftEdge", onMouse, 0 );
		if(waitKey(30) >= 0) break;
	}
	leftRunning=false;
}
void ThreadCaptureRightImageAndShow(PVOID param){
	VideoCapture cap(0);
	if(!cap.isOpened()){
		std::cout<<"right eye camera open failed";
		return ;
	}
	Mat rightBuffer;
	Mat rightCaptureBuffer;
	Mat rightCaptureBuffer2;
	Mat rightEdgeBuffer1;
	Mat rightEdgeBuffer2;
	rightRunning=true;
	while(true)
	{
		cap>>rightCaptureBuffer2;

		//pyrDown(rightCaptureBuffer,rightCaptureBuffer2);
		EnterCriticalSection(&cs_rightImg);
		pyrDown(rightCaptureBuffer2,rightEyeImg);
		LeaveCriticalSection(&cs_rightImg);

		Canny(rightCaptureBuffer2,rightEdgeBuffer2,150,50);
		//pyrDown(rightEdgeBuffer1,rightEdgeBuffer2);
		EnterCriticalSection(&cs_rightEdgeImg);
		pyrDown(rightEdgeBuffer2,rightEdgeImg);
		LeaveCriticalSection(&cs_rightEdgeImg);

		rightBuffer=rightEyeImg.clone();

		EnterCriticalSection(&cs_rightImgResult);
		mergeImage(rightBuffer,rightEyeResult);
		LeaveCriticalSection(&cs_rightImgResult);
		imshow("right",rightBuffer);
		if(waitKey(30) >= 0) break;
	}
	rightRunning=false;
}

int	main(int argc,char*argv[]){
	patchPairsReady=CreateSemaphore(NULL, 0, 1,NULL);
	InitializeCriticalSection(&cs_leftImg);
	InitializeCriticalSection(&cs_rightImg);
	InitializeCriticalSection(&cs_leftImgResult);
	InitializeCriticalSection(&cs_rightImgResult);
	InitializeCriticalSection(&cs_leftEdgeImg);
	InitializeCriticalSection(&cs_rightEdgeImg);
	_beginthread(ThreadCaptureLeftImageAndShow,0,NULL);
	_beginthread(ThreadCaptureRightImageAndShow,0,NULL);
	_beginthread(ThreadSearchCorrespondence,0,NULL);
	_beginthread(ThreadEstimateDistance,0,NULL);
	Sleep(1000000);
}