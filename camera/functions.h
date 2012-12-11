//#ifndef FUNCTION_H
//#define FUNCTION_H
//#include <opencv2/opencv.hpp>
//#include <cstdlib>
//
//using namespace cv;
//using namespace std;
//typedef unsigned int uint;
//extern Mat rightEyeResult,leftEyeResult;
//extern Mat rightEyeImage,leftEyeImage;
//static const char alphanum[] =
//	"0123456789"
//	"!@#$%^&*"
//	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//	"abcdefghijklmnopqrstuvwxyz";
//
//int stringLength = sizeof(alphanum) - 1;
//
//char* genRandomString()
//{
//	char *retVal=(char *)malloc(10);
//	for (uint i=0;i<10;++i)
//	{
//		retVal[i]=alphanum[rand() % stringLength];
//	}
//	return retVal;
//}
//
//namespace bee{
//	inline double invSqrt(double x)
//	{
//		union
//		{
//			double x;
//			int i;
//		} convert;
//
//		convert.x = x;
//		double xhalf = 0.5f * x;
//		convert.i = 0x5f3759df - (convert.i >> 1);
//		x = convert.x;
//		x = x * (1.5f - xhalf * x * x);
//		return x;
//	}
//	inline double abs(double a){
//		return a > 0.0f ? a : -a;
//	}
//	inline int abs(int a){
//		return a > 0 ? a : -a;
//	}
//	double euclideanDistance(const Vec3b&a,const Vec3b &b){
//		return sqrtf((a[0]-b[0])*(a[0]-b[0])+(a[1]-b[1])*(a[1]-b[1])+(a[2]-b[2])*(a[2]-b[2]));
//	}
//
//	double hamiltonDistance(const Vec3b&a,const Vec3b &b){
//		return abs(a[0]-b[0])+abs(a[1]-b[1])+abs(a[2]-b[2]);
//	}
//	bool shouldCalc(const Vec3b&a){
//		if(a[0]==0&&a[1]==0&&a[2]==0){
//			return false;
//		}
//		return true;
//	}
//	template<typename t>
//	double matrixDistance(const Mat &a,const Mat &b,uint a_x,uint a_y,uint b_x,uint b_y,uint rows,uint cols){
//		uint a_rows=a.rows,a_cols=a.cols,b_rows=b.rows,b_cols=b.cols;
//		if(a_x>=a_rows||a_y>=a_cols||a_x+rows>a_rows||a_y+rows>a_cols||b_x>=b_rows||b_y>=b_cols||b_x+rows>b_rows||b_y+cols>b_cols){
//			throw new exception;
//		}
//		double square_sum=0;
//		for (uint i=0;i<rows;++i)
//		{
//			t *a_row_ptr=a.ptr<t>(a_x+i)+a_y;
//			t *b_row_ptr=b.ptr<t>(b_x+i)+b_y;
//			for(uint j=0;j<cols;++j){
//				//if(shouldCalc(*a_row_ptr)&&shouldCalc(*b_row_ptr)){
//					square_sum+=hamiltonDistance(*(a_row_ptr++),*(b_row_ptr++));
//				//}
//			}
//		}
//		return square_sum/(rows*cols);
//	}
//
//	Point2i findCorrespondArea(const Mat&a,const Mat&b,uint x,uint y,uint rows,uint cols,uint range){
//		uint min_x=0,min_y=0;
//		double min_distance=99999999999999.9;
//		uint range_left=y-range<0?0:y-range;
//		uint range_right=y;
//		uint range_top=x,range_bottom=x+3;
//
//		//rectangle(leftEyeResult,Point(y,x),Point(y+cols,x+rows),Scalar(255,255,255));
//		for (uint i=range_top;i<range_bottom;++i)
//		{
//			for (uint j=range_left;j<range_right;++j)
//			{
//				try
//				{
//					/*rectangle(rightEyeResult,Point(j,i),Point(j+cols,i+rows),Scalar(255,255,255));*/
//					double distance=matrixDistance<const Vec3b>(a,b,x,y,i,j,rows,cols);
//					if (distance<min_distance)
//					{
//						min_distance=distance;
//						min_x=i;
//						min_y=j;
//					}
//				}
//				catch (exception* e)
//				{
//					continue;
//				}
//			}
//			
//		}
//		//rectangle(rightEyeResult,Point(min_y,min_x),Point(min_y+cols,min_x+rows),Scalar(255,255,255));
//		return Point2i(min_x,min_y);
//	}
//	Vec3d operator/ (const Vec3d & input, double scale){
//		return Vec3d(input.val[0]/scale,input.val[1]/scale,input.val[2]/scale);
//	}
//	void showDoubleMatrix(const Mat&ret){
//		Mat showRet;
//		showRet.create(ret.rows,ret.cols,CV_8UC3);
//		for (uint i=0;i<showRet.rows;++i)
//		{
//			for (uint j=0;j<showRet.cols;++j)
//			{
//				showRet.at<Vec3b>(i,j)=Vec3b((ret.at<Vec3d>(i,j)));
//			}
//		}
//		imshow(genRandomString(),showRet);
//
//		waitKey(0);
//	}
//	void calcMatrixAverageBasedOnLeftAvgVal(const Mat&input,Mat & avgMat,uint windowSize,uint row,uint col){
//		uint padding=(windowSize-1)/2;
//		uint baseCol=col-padding-1;
//		Vec3d* leftAvg=avgMat.ptr<Vec3d>(row)+col-1;
//		Vec3d colSum(0,0,0),leftColSum(0,0,0);
//		const Vec3b *row_ptr;
//		for (uint i=0;i<windowSize;++i)
//		{
//			row_ptr=input.ptr<const Vec3b>(row+i)+baseCol;
//			leftColSum.val[0]+=row_ptr->val[0];
//			leftColSum.val[1]+=row_ptr->val[1];
//			leftColSum.val[2]+=row_ptr->val[2];
//
//			row_ptr=row_ptr+windowSize;
//			colSum.val[0]+=row_ptr->val[0];
//			colSum.val[1]+=row_ptr->val[1];
//			colSum.val[2]+=row_ptr->val[2];
//		}
//		*(avgMat.ptr<Vec3d>(row)+col)=*leftAvg+(colSum-leftColSum)/(windowSize*windowSize);
//	}
//	void calcMatrixAverageBasedOnUpperAvgVal(const Mat&input,Mat & avgMat,uint windowSize,uint row,uint col){
//		uint padding=(windowSize-1)/2;
//		uint baseRow=row-padding-1;
//		Vec3d* upperAvg=avgMat.ptr<Vec3d>(row-1)+col;
//		Vec3d rowSum(0,0,0),baseRowSum(0,0,0);
//		const Vec3b *base_row_ptr=input.ptr<const Vec3b>(baseRow);
//		const Vec3b *row_ptr=input.ptr<const Vec3b>(baseRow+windowSize);
//		for (uint i=0;i<windowSize;++i)
//		{
//			baseRowSum.val[0]+=base_row_ptr->val[0];
//			baseRowSum.val[1]+=base_row_ptr->val[1];
//			baseRowSum.val[2]+=base_row_ptr->val[2];
//			++base_row_ptr;
//
//			rowSum.val[0]+=row_ptr->val[0];
//			rowSum.val[1]+=row_ptr->val[1];
//			rowSum.val[2]+=row_ptr->val[2];
//			++row_ptr;
//		}
//		*(avgMat.ptr<Vec3d>(row)+col)=*upperAvg+(rowSum-baseRowSum)/(windowSize*windowSize);
//	}
//	Mat smoothMat(const Mat &input,uint windowSize){
//		int paddingSize=(windowSize-1)/2;
//		Mat retVal;
//		retVal.create(input.rows,input.cols,CV_64FC3);
//		for (uint k=paddingSize;k<input.rows-paddingSize;++k)
//		{
//			for (uint p=paddingSize;p<input.cols-paddingSize;++p)
//			{
//				Vec3d tmpSum(0,0,0);
//				for (int i=-paddingSize;i<=paddingSize;++i)
//				{
//					const Vec3b *row_ptr=input.ptr<Vec3b>(k+i);
//					for(int j=-paddingSize;j<=paddingSize;++j){
//						const Vec3b *col_ptr=row_ptr+(p+j);
//						tmpSum+=*col_ptr;
//					}
//				}
//				retVal.at<Vec3d>(k,p)=tmpSum/(windowSize*windowSize);
//			}
//		}
//		return retVal;
//	}
//	Mat calcAverageMatrix(const Mat&input,uint windowSize){
//		int paddingSize=(windowSize-1)/2;
//		Mat retVal;
//		retVal.create(input.rows,input.cols,CV_64FC3);
//
//		Vec3d tmpSum(0,0,0);
//		Vec3d firstAverageVec3(0,0,0);
//		
//		showDoubleMatrix(smoothMat(input,windowSize));
//		for (int i=-paddingSize;i<=paddingSize;++i)
//		{
//			const Vec3b *row_ptr=input.ptr<Vec3b>(paddingSize+i);
//			for(int j=-paddingSize;j<=paddingSize;++j){
//				const Vec3b *col_ptr=row_ptr+(paddingSize+j);
//				tmpSum+=*col_ptr;
//			}
//		}
//		/*for(uint i=0;i<windowSize;++i){
//			const Vec3b *row_ptr=input.ptr<Vec3b>(paddingSize+i);
//			for(uint j=0;j<windowSize;++j){
//				const Vec3b *col_ptr=row_ptr+j+paddingSize;
//				tmpSum+=*col_ptr;
//			}
//		}*/
//		firstAverageVec3=tmpSum/(windowSize*windowSize);
//		retVal.at<Vec3d>(paddingSize,paddingSize)=firstAverageVec3;
//		for (uint i=paddingSize+1;i<input.cols-paddingSize;++i)
//		{
//			calcMatrixAverageBasedOnLeftAvgVal(input,retVal,windowSize,paddingSize,i);
//		}
//		for (uint i=paddingSize;i<input.cols-paddingSize;++i)
//		{
//			for (uint j=paddingSize+1;j<input.rows-paddingSize;++j)
//			{
//				calcMatrixAverageBasedOnUpperAvgVal(input,retVal,windowSize,j,i);
//			}
//		}
//		return retVal;
//	}
//}
//
//#endif