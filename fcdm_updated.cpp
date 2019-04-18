
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "image/Image.h"
#include "image/ImageIO.h"
#include "fitline/LFLineFitter.h"
#include "fdcm/LMLineMatcher.h"
#include <string>
#include <Fitline\Point1.h>

#include <iostream>

using namespace cv;
using namespace std;


string intTostring(int i){
	stringstream s;
	s << i;
	return s.str();
}

void prepose_img(Mat &img){
	cvtColor(img, img, CV_RGB2GRAY);
	blur(img, img, Size(3,3));
	Canny(img, img, 120, 150);
	//blur(img, img, Size(3, 3));
}
void prepose1_img(Mat &templateEdgeMap){
	cvtColor(templateEdgeMap, templateEdgeMap, CV_BGR2GRAY);
	threshold(templateEdgeMap, templateEdgeMap, 100, 255, 1);
	//flip(templateEdgeMap, templateEdgeMap, 1);
}

int main(){
	double ti = (double)getTickCount();
//	Mat templateEdgeMap = imread("template/handdrawn_bottle.pgm", -1);
//	
//	//templateEdgeMap = 255 - templateEdgeMap;
//	Mat query = imread("query/ceazanne_edges.pgm", -1);
//	
//	Mat queryColor = imread("query/ceazanne.jpg", -1);
////	prepose_img(query);
//	Mat temp = queryColor.clone();
//	//templateEdgeMap.convertTo(templateEdgeMap, CV_64F);



	Mat templateEdgeMap = imread("D:\\_temp\\dis\\apple\\apl.jpg", -1);

	//templateEdgeMap = 255 - templateEdgeMap;
	Mat query = imread("D:\\_temp\\dis\\apple\\3.jpg", -1);
	Mat queryColor = imread("D:\\_temp\\dis\\apple\\3.jpg", -1);


	prepose1_img(templateEdgeMap);
	prepose_img(query);
	Mat temp = queryColor.clone();
	

	size_t width, height;
	unsigned char *data;

	width = templateEdgeMap.cols;
	height = templateEdgeMap.rows;
	//data = templateEdgeMap;
	Image<uchar> inputImage;
	inputImage.Resize(width, height, false);
	cout << height << "   " << width << endl;

	int    row, col;
	for (col = 0; col < width; col++)
	{
		for (row = 0; row < height; row++)
		{
			inputImage.Access(col, row) = templateEdgeMap.at<uchar>(row,col);
		}
	}

	imshow("abc", templateEdgeMap);

	

	double sigma_fit_a_line = .5;
	double sigma_find_support = .5;
	double max_gap = 2.0;
	double nLinesToFitInStage1 = 300;
	double nTrialsPerLineInStage1 = 100;
	double nLinesToFitInStage2 = 100000;
	double nTrialsPerLineInStage2 = 1;
	//
	int nLayer = 2;
	int nLinesToFitInStage[2];
	int nTrialsPerLineInStage[2];
	nLinesToFitInStage[0] = (int)floor(nLinesToFitInStage1);
	nLinesToFitInStage[1] = (int)floor(nLinesToFitInStage2);
	nTrialsPerLineInStage[0] = (int)floor(nTrialsPerLineInStage1);
	nTrialsPerLineInStage[1] = (int)floor(nTrialsPerLineInStage2);
	//

	//myc a;
	//a.sum_two(1, 2);
	LFLineFitter lf;
	lf.Configure(sigma_fit_a_line, sigma_find_support, max_gap, nLayer, nLinesToFitInStage, nTrialsPerLineInStage);
	lf.Init();
	cout << lf.rNLineSegments() << endl;
	lf.FitLine(&inputImage);
	double *out;
	cout << lf.rNLineSegments() << endl;
	Mat lineRep(lf.rNLineSegments(), 4, CV_8UC1);
	//= mxCreateDoubleMatrix(lf.rNLineSegments(), 4, mxREAL);

	for (int i = 0; i<lf.rNLineSegments(); i++)
	{

		lineRep.at<uchar>(i, 0) = lf.outEdgeMap_[i].sx_;
		lineRep.at<uchar>(i, 1) = lf.outEdgeMap_[i].sy_;
		lineRep.at<uchar>(i, 2) = lf.outEdgeMap_[i].ex_;
		lineRep.at<uchar>(i, 3) = lf.outEdgeMap_[i].ey_;
	}


	Image<uchar> *debugImage = lf.ComputeOuputLineImage(&inputImage);

	Mat lineMap(height, width, CV_8UC1);

	for (col = 0; col < lineMap.cols; col++)
	{
		for (row = 0; row < lineMap.rows; row++)
		{
			lineMap.at<uchar>(row, col)=debugImage->Access(col, row);
		}
	}



	//2nd part

	

	size_t width1, height1;
	//double *data;
	width1 = query.cols;
	height1 = query.rows;
	//data = mxGetPr(prhs[0]);
	Image<uchar> inputImage1;
	inputImage1.Resize(width1, height1, false);

	for (col = 0; col < width1; col++)
	{
		for (row = 0; row < height1; row++)
		{
			inputImage1.Access(col, row) = query.at<uchar>(row, col);
		}
	}

	double sigma_fit_a_line1=0.5;
	double sigma_find_support1=0.5;
	double max_gap1=2.0;
	double nLinesToFitInStage11=0;
	double nTrialsPerLineInStage11=0;
	double nLinesToFitInStage21=100000;
	double nTrialsPerLineInStage21=1;

	int nLayer1 = 2;
	int nLinesToFitInStage_n[2];
	int nTrialsPerLineInStage_n[2];
	nLinesToFitInStage_n[0] = (int)floor(nLinesToFitInStage11);
	nLinesToFitInStage_n[1] = (int)floor(nLinesToFitInStage21);
	nTrialsPerLineInStage_n[0] = (int)floor(nTrialsPerLineInStage11);
	nTrialsPerLineInStage_n[1] = (int)floor(nTrialsPerLineInStage21);

	LFLineFitter lf1;
	lf1.Configure(sigma_fit_a_line1, sigma_find_support1, max_gap1, nLayer1, nLinesToFitInStage_n, nTrialsPerLineInStage_n);
	lf1.Init();
	lf1.FitLine(&inputImage1);

	double nDirection=60;
	double directionCost=0.5;
	double maxCost=30;
	double matchingScale=1.0;
	double dbScale=0.6761;
	double baseSearchScale=1.20;
	double minSearchScale=-7;
	double maxSearchScale=0;
	double baseSearchAspect=1.1;
	double minSearchAspect=-1;
	double maxSearchAspect=1;
	double searchStepSize=2;
	double searchBoundarySize=2;
	double minCostRatio=1.0;

	double maxThreshold = 0.23;
	LMLineMatcher lm;

	// Load configuration
	lm.nDirections_ = (int)(nDirection);
	lm.directionCost_ = (float)(directionCost);
	lm.maxCost_ = maxCost;
	lm.scale_ = matchingScale;
	lm.db_scale_ = dbScale;
	lm.baseSearchScale_ = baseSearchScale;
	lm.minSearchScale_ = (int)(minSearchScale);
	lm.maxSearchScale_ = (int)(maxSearchScale);
	lm.baseSearchAspect_ = baseSearchAspect;
	lm.minSearchAspect_ = (int)(minSearchAspect);
	lm.maxSearchAspect_ = (int)(maxSearchAspect);
	lm.searchStepSize_ = (int)(searchStepSize);
	lm.searchBoundarySize_ = (int)(searchBoundarySize);
	lm.minCostRatio_ = minCostRatio;

	Mat tempate = lineRep.clone();

	int m = lineRep.rows;
	int n = lineRep.cols;
	int nTemplate = max(m, n);
	lm.ndbImages_ = nTemplate;
	lm.dbImages_ = new EIEdgeImage[lm.ndbImages_];

	Mat tempShape;
	for (int i = 0; i<nTemplate; i++)
	{
		tempShape = tempate.clone();
		//Mat pTempShape;// = mxGetPr(tempShape);
		//tempShape.convertTo(pTempShape, CV_64FC1);
		int nRow = tempShape.rows;
		int nCol = tempShape.cols;
		//mexPrintf("( m , n ) = ( %d , %d )\n",nRow,nCol);
		lm.dbImages_[i].SetNumDirections(lm.nDirections_);
		lm.dbImages_[i].Read(tempShape, nRow);

		/*int nLines_ = nRow;
		LFLineSegment *lines_ = new LFLineSegment[nLines_];
		int width_ = 0;
		int height_ = 0;
		for (int ii = 0; ii<nLines_; ii++)
		{
			lines_[ii].sx_ = tempShape.at<uchar>(ii, 0);
			lines_[ii].sy_ = tempShape.at<uchar>(ii, 1);
			lines_[ii].ex_ = tempShape.at<uchar>(ii, 2);
			lines_[ii].ey_ = tempShape.at<uchar>(ii, 3);
			if (max(lines_[ii].sx_, lines_[ii].ex_) > 1.0*width_)
				width_ = (int)max(lines_[ii].sx_, lines_[ii].ex_);
			if (max(lines_[ii].sy_, lines_[ii].ey_) > 1.0*height_)
				height_ = (int)max(lines_[ii].sy_, lines_[ii].ey_);
		}
*/

		lm.dbImages_[i].Scale(lm.scale_*lm.db_scale_);
	}

	vector< vector<LMDetWind> > detWindArrays;
	detWindArrays.clear();
	lm.SingleShapeDetectionWithVaryingQuerySize(lf1, maxThreshold, detWindArrays);

	int last = detWindArrays.size() - 1;
	int nDetWindows = detWindArrays[last].size();

	//double *out;

	Mat detWinds(nDetWindows,6,CV_64FC1);
	//plhs[0] = mxCreateDoubleMatrix(nDetWindows, 6, mxREAL);
	//out = mxGetPr(plhs[0]);
	for (int i = 0; i<nDetWindows; i++)
	{
		detWinds.at<double>(i,0) = 1.0*detWindArrays[last][i].x_;
		detWinds.at<double>(i, 1) = 1.0*detWindArrays[last][i].y_;
		detWinds.at<double>(i, 2) = 1.0*detWindArrays[last][i].width_;
		detWinds.at<double>(i, 3) = 1.0*detWindArrays[last][i].height_;
		detWinds.at<double>(i, 4) = 1.0*detWindArrays[last][i].cost_;
		detWinds.at<double>(i, 5) = 1.0*detWindArrays[last][i].count_;
	}

	Scalar colour(0, 255, 0);

	RotatedRect rRect;
	Rect brect;
	vector<Mat> roi;

	for (int a = 0; a < (detWinds.rows); a++){
		int sx = detWinds.at<double>(a, 0);
		int ex = sx + detWinds.at<double>(a, 2);
		int sy = detWinds.at<double>(a, 1);
		int ey = sy + detWinds.at<double>(a, 3);
		line(queryColor, Point(sx, sy), Point(ex, sy), colour, 3, 8);
		line(queryColor, Point(sx, ey), Point(ex, ey), colour, 3, 8);
		line(queryColor, Point(sx, sy), Point(sx, ey), colour, 3, 8);
		line(queryColor, Point(ex, sy), Point(ex, ey), colour, 3, 8);
		Point center(abs(sx + ex) / 2, abs(sy + ey) / 2);
		rRect = RotatedRect(center, Size(abs(sx - ex),abs( sy - ey)), 0);
		brect = rRect.boundingRect();
		roi.push_back(temp(brect));
	}

	
	imshow("final ", queryColor);
	
	for (int i = 0; i < roi.size(); i++){
		imwrite("roi/" + intTostring(i) + ".jpg", roi.at(i));
	}
	imwrite("D:\\_temp\\dis\\apple\\r3.jpg", queryColor);
	
	ti = ((double)getTickCount() - ti) / getTickFrequency();
	cout << "time in second= " << ti << endl;
	waitKey(0);
	return 0;
}




/**
* @file CannyDetector_Demo.cpp
* @brief Sample code showing how to detect edges using the Canny Detector
* @author OpenCV team
*/

//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <stdlib.h>
//#include <stdio.h>
//
//using namespace cv;
//
///// Global variables
//
//Mat src, src_gray;
//Mat dst, detected_edges;
//
//int edgeThresh = 1;
//int lowThreshold, uprThreshold;
//int const max_lowThreshold = 255;
//int ratio = 3;
//int kernel_size = 3;
//const char* window_name = "Edge Map";
//
///**
//* @function CannyThreshold
//* @brief Trackbar callback - Canny thresholds input with a ratio 1:3
//*/
//static void CannyThreshold(int, void*)
//{
//	/// Reduce noise with a kernel 3x3
//	blur(src_gray, detected_edges, Size(3, 3));
//
//	/// Canny detector
//	Canny(detected_edges, detected_edges, lowThreshold, uprThreshold);
//
//	/// Using Canny's output as a mask, we display our result
//	dst = Scalar::all(0);
//
//	src.copyTo(dst, detected_edges);
//	imshow(window_name, dst);
//}
//
//
///**
//* @function main
//*/
//int main(int, char** argv)
//{
//	/// Load an image
//	src = imread("beach.jpg",1);
//
//	if (!src.data)
//	{
//		return -1;
//	}
//
//	/// Create a matrix of the same type and size as src (for dst)
//	dst.create(src.size(), src.type());
//
//	/// Convert the image to grayscale
//	cvtColor(src, src_gray, COLOR_BGR2GRAY);
//
//	/// Create a window
//	namedWindow(window_name, WINDOW_AUTOSIZE);
//
//	/// Create a Trackbar for user to enter threshold
//	createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
//	createTrackbar("Max Threshold:", window_name, &uprThreshold, max_lowThreshold, CannyThreshold);
//
//	/// Show the image
//	CannyThreshold(0, 0);
//
//	/// Wait until user exit program by pressing a key
//	waitKey(0);
//
//	return 0;
//}
