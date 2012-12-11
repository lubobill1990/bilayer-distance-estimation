#ifndef COMMON_H
#define COMMON_H
#include <opencv2/opencv.hpp>
#include <cstdlib>

using namespace cv;
using namespace std;
typedef unsigned int uint;
typedef enum _direction{LEFT,RIGHT}Direction;
extern const int patchSize;
const int searchWidth=80;

extern Mat rightEyeImg,
	leftEyeImg,
	rightEyeResult,
	leftEyeResult;

static const char alphanum[] =
	"0123456789"
	"!@#$%^&*"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

char* genRandomString()
{
	char *retVal=(char *)malloc(10);
	for (int i=0;i<10;++i)
	{
		retVal[i]=alphanum[rand() % stringLength];
	}
	return retVal;
}
void mergeImage(Mat &dist,Mat &src){
	if(dist.rows==src.rows&&dist.cols==src.cols){
		for(int row=0;row<dist.rows;++row){
			uchar *s=src.ptr(row);
			uchar *d=dist.ptr(row);
			for (int col=0;col<dist.cols;++col)
			{
				if(*s!=0){
					*d=*s;
					*(d+1)=*s;
					*(d+2)=*s;
				}
				d+=3;
				s++;
			}
		}
	}
}
void matSetZero(Mat &src){
	uchar *ptr;
	for (int i=0;i<src.rows;++i)
	{
		ptr=src.ptr(i);
		for(int j=0;j<src.cols;++j){
			*ptr=0;
			++ptr;
		}
	}
}
inline double hamiltonDistance(const Vec3b&a,const Vec3b &b){
	return abs(a[0]-b[0])+abs(a[1]-b[1])+abs(a[2]-b[2]);
}
inline void vec3dAdd(Vec3d&a,const Vec3b&b){
	a[0]+=b[0];
	a[1]+=b[1];
	a[2]+=b[2];
}
double getMatrixDistance(const Mat& a, const Mat& b, Point2i alt,Point2i blt,int rows,int cols){
	int a_rows=a.rows,a_cols=a.cols,b_rows=b.rows,b_cols=b.cols;
	if(alt.x+rows>a_rows||alt.y+cols>a_cols||blt.x+rows>b_rows||blt.y+cols>b_cols||alt.x<0||alt.y<0||blt.x<0||blt.y<0){
		return 100000000.0;
	}
	//Mat roiTmpA;
	//roiTmpA.create(rows,cols,CV_8UC3);
	//Mat roiTmpB=roiTmpA.clone();
	//Vec3d roiASum(0,0,0),roiBSum(0,0,0);
	//for (int i=0;i<rows;++i)
	//{
	//	const Vec3b * a_row_ptr=a.ptr<const Vec3b>(alt.x+i)+alt.y;
	//	const Vec3b * b_row_ptr=b.ptr<const Vec3b>(blt.x+i)+blt.y;
	//	Vec3b *tmpARowPtr=roiTmpA.ptr<Vec3b>(i);
	//	Vec3b *tmpBRowPtr=roiTmpB.ptr<Vec3b>(i);
	//	
	//	for(int j=0;j<cols;++j){
	//		*(tmpARowPtr)=*(a_row_ptr++);
	//		*(tmpBRowPtr)=*(b_row_ptr++);
	//		vec3dAdd(roiASum,*tmpARowPtr);
	//		vec3dAdd(roiBSum,*tmpBRowPtr);
	//	}
	//}
	//for (int i=0;i<rows;++i)
	//{
	//	const 
	//	for (int j=0;j<cols;++j)
	//	{

	//	}
	//}
	double square_sum=0;//
	for (int i=0;i<rows;++i)
	{
		const Vec3b * a_row_ptr=a.ptr<const Vec3b>(alt.x+i)+alt.y;
		const Vec3b * b_row_ptr=b.ptr<const Vec3b>(blt.x+i)+blt.y;
		for(int j=0;j<cols;++j){
			square_sum+=hamiltonDistance(*(a_row_ptr++),*(b_row_ptr++));
		}
	}
	return square_sum/(rows*cols);
}
Point2i getCorrespondPatch(const Mat& a, const Mat& b, Point2i patchLeftTop, Direction direction,int searchAreaWidth,int searchAreaHeight,int patchSize,double &minimalMatDistance,double &distanceAvg){
	Point2i retPoint;
	double minDistance=2000000000.0;
	int range_left,range_right,range_top,range_bottom;
	if(direction==LEFT){//到另一图像里的左方找相似区域
		range_left=patchLeftTop.y-searchAreaWidth<0?0:patchLeftTop.y-searchAreaWidth;
		range_right=patchLeftTop.y;
	}else if(direction==RIGHT){//到另一图像里的右方找相似区域
		range_left=patchLeftTop.y;
		range_right=patchLeftTop.y+searchAreaWidth>=b.cols?b.cols:patchLeftTop.y+searchAreaWidth;
	}
	range_top=patchLeftTop.x;
	range_bottom=patchLeftTop.x+searchAreaHeight;
	double distanceSum=0;
	double tmpDistance=0;
	for (int i=range_top;i<range_bottom;++i)
	{
		for (int j=range_left;j<range_right;++j)
		{
			//a中的某一块patch到b中相应位置计算距离
			tmpDistance=getMatrixDistance(a,b,patchLeftTop,Point2i(i,j),patchSize,patchSize);
			distanceSum+=tmpDistance;
			if (tmpDistance<minDistance)
			{
				minDistance=tmpDistance;
				retPoint=Point2i(i,j);
			}
		}
	}
	//如果除数为0，则distanceSum一定为0，所以不会出错
	distanceAvg=distanceSum==0?0:distanceSum/((range_right-range_left)*(range_bottom-range_top));
	minimalMatDistance=minDistance;
	return retPoint;
}
Point2i getCorrespondPatchInRightImg(const Mat& a, const Mat& b, Point2i regionLeftTop,double &minimalMatDistance,double &distanceAvg){
	return getCorrespondPatch(a,b,regionLeftTop,LEFT,searchWidth,3,patchSize,minimalMatDistance,distanceAvg);
}
Point2i getCorrespondPatchInLeftImg(const Mat& a, const Mat& b, Point2i regionLeftTop,double &minimalMatDistance,double &distanceAvg){
	return getCorrespondPatch(a,b,regionLeftTop,RIGHT,searchWidth,3,patchSize,minimalMatDistance,distanceAvg);
}
bool hasEdgeInPatch(const Mat & a, Point2i patchLeftTop,int patchSize){
	int rangeLeft=patchLeftTop.y;
	int rangeRight=patchLeftTop.y+patchSize;
	int rangeTop=patchLeftTop.x;
	int rangeBottom=patchLeftTop.x+patchSize;
	for (int i=rangeTop;i<rangeBottom;++i)
	{
		const uchar *row_ptr=a.ptr(i)+rangeLeft;
		for (int j=rangeLeft;j<rangeRight;++j)
		{
			if(*(row_ptr++)>0){
				return true;
			}
		}
	}
	return false;
}
#endif