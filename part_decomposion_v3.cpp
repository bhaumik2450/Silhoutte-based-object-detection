#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include<tuple>

using namespace cv;
using namespace std;

RNG rng(500);
Mat prepros(Mat img);

//void part_decom(Mat sketch_gray);
tuple<vector<Point>, vector<Point>, vector<Point>, vector<double>> defects_points(Mat img);
vector< vector<Point>> remaining_contours(vector< vector<Point>>contours, Point a);
vector< vector<Point>> matchcontour(vector< vector<Point>>contours, Point a);
vector<Point> find_cupoint_temp(vector<vector<Point>>contours, vector<Point>def_points);

Point find_cupoint(vector<vector<Point>>contours, vector<Point>def_points);
vector<Point> final_cutpoint(vector<vector<Point>>contours, Point pt);
Mat reg_int(Mat sketch, Point p, int a, int b);
//Mat sketch1;

string intTostring(int i){
	stringstream s;
	s << i;
	return s.str();
}



int main(){

	Mat sketch = imread("D:/_temp/data/1.jpg", 1);// imshow("source", sketch);
	namedWindow("source", WINDOW_NORMAL);
	Mat sketch1, final_sketch;
	sketch.copyTo(final_sketch);
	sketch.copyTo(sketch1);
	Mat sketch_gray;
	cvtColor(sketch, sketch_gray, CV_RGB2GRAY);
	Mat proc_img = prepros(sketch);
	tuple<vector<Point>, vector<Point>, vector<Point>, vector<double>>defects = defects_points(proc_img);

	vector<Point>start_points = get<0>(defects);
	vector<Point>end_points = get<1>(defects);
	vector<Point>def_points = get<2>(defects);
	vector<double> dep = get<3>(defects);

	vector<Point>all_points;
	Point x, y;
	for (int a = 0; a<def_points.size(); a++){
		circle(proc_img, def_points.at(a), 5, Scalar(255,255,255), 1, 8, 0);
	}
	
	int cmp = 0;

	if (def_points.size() > 0){
		for (int i = 0; i < def_points.size(); i++){
			
			Scalar colour(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			vector <vector<Point>>contours, temp_contour;
			vector<Vec4i>hierarchy;
			int a = 150, b = 150;

			Point center = def_points.at(i);
			circle(sketch1, center, 5, colour, 1, 8, 0);

			//creating roi for processing
			/*RotatedRect rRect = RotatedRect(def_points.at(i), Size(a, b), 90);
			Rect brect = rRect.boundingRect();
			Mat roi = sketch(brect);*/

			Mat roi = reg_int(sketch, def_points.at(i), a, b);

			//	rectangle(sketch1, brect, Scalar(255, 0, 0));
			roi = prepros(roi);
			findContours(roi, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, def_points.at(i) - Point(a / 2, b / 2));
			for (size_t ii = 0; ii < contours.size(); ii++){
				drawContours(sketch1, contours, (int)ii, colour, 1, 8, hierarchy, 0, Point(0, 0));
			}

			//find remaining contorus
			temp_contour = matchcontour(contours, def_points.at(i));
			contours = remaining_contours(contours, def_points.at(i));
			
			vector<Point> temp_cutpoint = find_cupoint_temp(temp_contour, def_points);
			if (temp_cutpoint.size() == 2){
				x = temp_cutpoint.at(0);
				y = temp_cutpoint.at(1);
				line(final_sketch, x, y, Scalar(0, 0, 0), 2, 8, 0);
				cmp = 1;
			}
			

			int stp = 0;
			while (contours.size() == 0 & stp != 2 & temp_cutpoint.size() != 2){
				a = a - ((abs(a / 20)) * 2);
				b = b - ((abs(b / 20)) * 2);
				roi = reg_int(sketch, def_points.at(i), a, b);
				roi = prepros(roi);
				//roi = reg_int(sketch,def_points.at(i),  a,  b);
				findContours(roi, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, def_points.at(i) - Point(a / 2, b / 2));
				for (size_t ii = 0; ii < contours.size(); ii++){
					drawContours(sketch1, contours, (int)ii, Scalar(0, 255, 0), 1, 8, hierarchy, 0, Point(0, 0));
				}
				stp++;
			}
			contours = remaining_contours(contours, def_points.at(i));


			if (contours.size()>0){
				//find cutpoint take consideration of def_points
				vector<Point> cutpointfinal;

				Point cutpoint = find_cupoint(contours, def_points);
				Point cutpoint1 = find_cupoint(contours, start_points);
				Point cutpoint2 = find_cupoint(contours, end_points);

				if (cutpoint != Point(0, 0) & temp_cutpoint.size() != 2){
					circle(sketch1, cutpoint, 5, colour, 1, 8, 0);
					line(final_sketch, def_points.at(i), cutpoint, Scalar(0, 0, 0), 2, 8);
					line(proc_img, def_points.at(i), cutpoint, Scalar(255,255,255), 2, 8);
					cmp = 1;
				}
				if (cutpoint == Point(0, 0) & cutpoint1 != Point(0, 0) & temp_cutpoint.size() != 2){
					line(final_sketch, def_points.at(i), cutpoint1, Scalar(0, 0, 0), 2, 8);
					line(proc_img, def_points.at(i), cutpoint1, Scalar(255, 255, 255), 2, 8);
					cmp = 1;
				}
				if (cutpoint == Point(0, 0) & cutpoint1 == Point(0, 0) & cutpoint2 != Point(0, 0) & temp_cutpoint.size() != 2){
					line(final_sketch, def_points.at(i), cutpoint2, Scalar(0, 0, 0), 2, 8);
					line(proc_img, def_points.at(i), cutpoint2, Scalar(255, 255, 255), 2, 8);
					cmp = 1;
				}



				if (cutpoint == Point(0, 0) & cutpoint1 == Point(0, 0) & cutpoint2 == Point(0, 0)  & temp_cutpoint.size() != 2){
					cutpointfinal = final_cutpoint(contours, def_points.at(i));
					for (int cc = 0; cc < cutpointfinal.size(); cc++){
						circle(sketch1, cutpointfinal.at(cc), 5, colour, 1, 8, 0);
						line(final_sketch, def_points.at(i), cutpointfinal.at(cc), Scalar(0, 0, 0), 2,8 );
						line(proc_img, def_points.at(i), cutpointfinal.at(cc), Scalar(255, 255, 255), 2, 8);
						cmp = 1;
					}
				}
			}
		}
	}   
	

	if (def_points.size() == 2 & cmp != 1){
		x = def_points.at(0);
		y = def_points.at(1);
		line(final_sketch, x, y, Scalar(0, 0, 0), 2, 8, 0);

	}

	cvtColor(final_sketch, final_sketch, CV_BGR2GRAY);

	threshold(final_sketch, final_sketch, 50, 255, THRESH_BINARY);


	vector <vector<Point>>contours, contours_resize;
	vector<Vec4i>hierarchy;
	findContours(final_sketch, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	int ind = 0;
	for (size_t i = 0; i< contours.size(); i++)
	{
		if (contours.at(i).size()>20){
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(sketch, contours, (int)i, color, -1, 8, vector<Vec4i>(), 0, Point());
			Mat drawing = Mat::zeros(sketch.size(), sketch.type());
			drawContours(drawing, contours, (int)i, color, -1, 8, vector<Vec4i>(), 0, Point());
			Rect rect = boundingRect(contours[i]);
			Mat roi = drawing(rect);
			imwrite("d:/_temp/roi/roi" + intTostring(ind) + ".jpg", roi);
			ind++;
			contours_resize.push_back(contours.at(i));
		}
	}

	//double largest_area=0, largest_contour_index=0;
	//for (int i = 0; i< contours_resize.size(); i++) // iterate through each contour. 
	//{
	//	double a = contourArea(contours_resize[i], false);  //  Find the area of contour
	//	if (a>largest_area)
	//	{
	//		largest_area = a;
	//		largest_contour_index = i;                //Store the index of largest contour
	//	}
	//}
	//final_sketch.copyTo(sketch);
	imshow("abcccccccccccc", sketch);
		imwrite("d:/_temp/roi/seg.jpg", sketch);
	//cout << largest_contour_index << endl;
	imshow("source", final_sketch);
	imshow("proc", proc_img);
	waitKey(0);
	return 0;
}

Mat reg_int(Mat sketch, Point p, int a, int b){
	RotatedRect rRect = RotatedRect(p, Size(a, b), 90);
	Rect brect = rRect.boundingRect();
	Mat roi = sketch(brect);
	//rectangle(sketch1, brect, Scalar(255, 0, 0));
	return roi;
}

vector<Point> final_cutpoint(vector<vector<Point>>contours, Point pt){
	vector<Point>cutpoint;
	Point temp1(0, 0), temp(0, 0), gbl(0, 0);
	double glb_dist1 = 5000, glb_dist2 = 5000;
	for (int i = 0; i < contours.size(); i++){
		double min_dist = 5000;
		vector<Point>x = contours.at(i);
		for (int j = 0; j < x.size(); j++){
			double dist = abs(norm(pt - x.at(j)));
			if (dist < min_dist){
				min_dist = dist;
				temp = x.at(j);
			}
		}

		if (min_dist < glb_dist2){
			glb_dist2 = glb_dist1;
			glb_dist1 = min_dist;
			temp1 = gbl;
			gbl = temp;
		}
	}
	if (temp1 != Point(0, 0)){
		cutpoint.push_back(gbl);
		cutpoint.push_back(temp1);
		return cutpoint;
	}
	else{
		cutpoint.push_back(gbl);
		return cutpoint;
	}

}

Point find_cupoint(vector<vector<Point>>contours, vector<Point>def_points){
	Point cutpoint;
	for (int cp = 0; cp < contours.size(); cp++){
		for (int pt = 0; pt < def_points.size(); pt++){
			vector<Point>xxx = contours.at(cp);
			for (int cpt = 0; cpt < xxx.size(); cpt++){
				if (xxx.at(cpt) == def_points.at(pt)){
					cutpoint = def_points.at(pt);
					return cutpoint;
				}
			}
		}
	}
	return 0;

}

vector<Point> find_cupoint_temp(vector<vector<Point>>contours, vector<Point>def_points){
	vector<Point> cutpoint,cr;
	vector<Point>x = contours.at(0);
	for (int dfp = 0; dfp < def_points.size(); dfp++){
		Point a = def_points.at(dfp);
		for (int cp = 0; cp < x.size(); cp++){
			if (x.at(cp) == a){
				cutpoint.push_back(def_points.at(dfp));
			}
		}
	}
	
	for (int i = 0; i < cutpoint.size(); i++){
		//Point p = cutpoint.at(i);
		int st = 0;
		if (i == 0){
			cr.push_back(cutpoint.at(i));
		}
		for (int j = 0; j < cr.size(); j++){
			if (cutpoint.at(i) == cr.at(j)){
				st = 1;
			}
		}
		if (st != 1){
			cr.push_back(cutpoint.at(i));
		}
	}

	return cr;

}


vector< vector<Point>> remaining_contours(vector< vector<Point>>contours, Point a){
	vector< vector<Point>>final_contours;
	for (size_t ii = 0; ii < contours.size(); ii++){

		vector<Point>pt = contours.at(ii);
		int chk = 0;
		for (int pti = 0; pti < pt.size(); pti++){
			if (pt.at(pti) == a)
			{
				chk = 1;
			}
		}
		if (chk != 1){
			final_contours.push_back(pt);
		}
	}
	return final_contours;
}

vector< vector<Point>> matchcontour(vector< vector<Point>>contours, Point a){
	vector< vector<Point>>final_contours;
	for (size_t ii = 0; ii < contours.size(); ii++){

		vector<Point>pt = contours.at(ii);
		int chk = 0;
		for (int pti = 0; pti < pt.size(); pti++){
			if (pt.at(pti) == a)
			{
				chk = 1;
			}
		}
		if (chk == 1){
			final_contours.push_back(pt);
		}
	}
	return final_contours;
}



Mat prepros(Mat img){
	Mat canny_output;
	cvtColor(img, img, CV_RGB2GRAY);
	Canny(img, canny_output, 50, 100, 3);
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(canny_output, canny_output, element);
	erode(canny_output, canny_output, element);
	return canny_output;
}

tuple<vector<Point>, vector<Point>, vector<Point>, vector<double>> defects_points(Mat img){
	Mat src = img.clone();
	vector<Point>a, b, c;
	vector <vector<Point>>contours;
	vector<Vec4i>hierarchy;
	double min = 500;
	vector<double>dep;
	int largest_area = 0;
	int largest_contour_index = 0;

	//threshold(src,thrsh_src, 100, 255, THRESH_BINARY);
	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> >hull(contours.size());
	vector<vector<int> >inthull(contours.size());
	vector<vector<Vec4i> >defects(contours.size());

	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(Mat(contours[i]), hull[i], false);
		convexHull(Mat(contours[i]), inthull[i], false);
		if (inthull[i].size()>3)
			convexityDefects(contours[i], inthull[i], defects[i]);
	}
	Mat drawing = Mat::zeros(src.size(), CV_8UC3);

	for (int i = 0; i< contours.size(); i++) // iterate through each contour. 
	{
		double a = contourArea(contours[i], false);  //  Find the area of contour
		if (a>largest_area)
		{
			largest_area = a;
			largest_contour_index = i;                //Store the index of largest contour
		}
	}
	drawContours(drawing, contours, (int)largest_contour_index, Scalar(0, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
	drawContours(drawing, hull, (int)largest_contour_index, Scalar(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
	double max_depth = 0;
	for (int cDefIt = 1; cDefIt < defects[largest_contour_index].size(); cDefIt++) {
		double depth = static_cast<double>(defects[largest_contour_index][cDefIt].val[3]) / 256;
		if (depth>max_depth){
			max_depth = depth;
		}

	}
	cout << "maxxx====" << max_depth << endl;
	for (int cDefIt = 1; cDefIt < defects[largest_contour_index].size(); cDefIt++) {

		int startIdx = defects[largest_contour_index][cDefIt].val[0]; Point ptStart(contours[largest_contour_index][startIdx]);

		int endIdx = defects[largest_contour_index][cDefIt].val[1]; Point ptEnd(contours[largest_contour_index][endIdx]);

		int farIdx = defects[largest_contour_index][cDefIt].val[2]; Point ptFar(contours[largest_contour_index][farIdx]);

		double depth = static_cast<double>(defects[largest_contour_index][cDefIt].val[3]) / 256;
		cout << "depth  " << depth << endl;


		if (depth > (max_depth / 8)){
			//cout << "depth = " << depth << endl;
			//display start points
			circle(drawing, ptStart, 5, CV_RGB(255, 0, 0), 2, 8);
			a.push_back(ptStart);
			//display all end points
			circle(drawing, ptEnd, 5, CV_RGB(0, 255, 0), 2, 8);
			b.push_back(ptEnd);
			//display all far points
			circle(drawing, ptFar, 5, CV_RGB(0, 0, 255), 2, 8);
			c.push_back(ptFar);
			if (depth < min)
				min = depth;
		}
	}
	dep.push_back(min);
	dep.push_back(max_depth);

	return make_tuple(a, b, c, dep);
}
