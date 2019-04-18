#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "image/Image.h"
#include "image/ImageIO.h"
#include "fitline/LFLineFitter.h"
#include "fdcm/LMLineMatcher.h"
#include <string>

#include <iostream>

using namespace cv;
using namespace std;

int main(){
	Mat templateEdgeMap = imread("Template/handdrawn_bottle.pgm",1);
	//templateEdgeMap.convertTo(templateEdgeMap, CV_64F);
	
	size_t width, height;
	unsigned char *data;
	
	width = templateEdgeMap.cols;
	height = templateEdgeMap.rows;
	//data = templateEdgeMap;
	Image<uchar> inputImage;
	inputImage.Resize(width, height, false);

	int    row,col; 
	for (col=0; col < width; col++)
	{
		for (row=0; row < height; row++)
		{			
			inputImage.Access(col, row) = templateEdgeMap.at<uchar>(row,col);
		}	
	}

	imshow("abc",templateEdgeMap);

	Mat query = imread("Query/ray_edges.pgm",1);
	Mat queryColor = imread("Query/ray.jpg",1);

	double sigma_fit_a_line=.5;
	double sigma_find_support=.5;
	double max_gap=2.0;
	double nLinesToFitInStage1=300;
	double nTrialsPerLineInStage1=100;
	double nLinesToFitInStage2=100000;
	double nTrialsPerLineInStage2=1;
	//
	int nLayer = 2;
	int nLinesToFitInStage[2];
	int nTrialsPerLineInStage[2];
	nLinesToFitInStage[0] = (int)floor(nLinesToFitInStage1);
	nLinesToFitInStage[1] = (int)floor(nLinesToFitInStage2);
	nTrialsPerLineInStage[0] = (int)floor(nTrialsPerLineInStage1);
	nTrialsPerLineInStage[1] = (int)floor(nTrialsPerLineInStage2);
	//
	LFLineFitter lf;
	lf.Configure(sigma_fit_a_line, sigma_find_support, max_gap, nLayer, nLinesToFitInStage, nTrialsPerLineInStage);
	lf.Init();
	lf.FitLine(&inputImage);
	double out;
	
	Mat lineRep(lf.rNLineSegments(), 4, CV_64FC1);
		//= mxCreateDoubleMatrix(lf.rNLineSegments(), 4, mxREAL);
	//out = lineRep.data;
	//for (int i = 0; i<lf.rNLineSegments(); i++)
	//{
	//	out[i + 0 * lf.rNLineSegments()] = lf.outEdgeMap_[i].sx_;
	//	out[i + 1 * lf.rNLineSegments()] = lf.outEdgeMap_[i].sy_;
	//	out[i + 2 * lf.rNLineSegments()] = lf.outEdgeMap_[i].ex_;
	//	out[i + 3 * lf.rNLineSegments()] = lf.outEdgeMap_[i].ey_;
	//}

	Image<uchar> *debugImage = lf.ComputeOuputLineImage(&inputImage);

	waitKey(0);
	return 0;
}


