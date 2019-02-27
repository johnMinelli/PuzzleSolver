//
//  Utils.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/9/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__Utils__
#define __PuzzleSolver__Utils__

#include <iostream>

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <sstream>
#include <dirent.h>
#include <vector>
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "params.h"
typedef std::vector<cv::Mat> imlist;

// Assuming Point represents a vector, return its magnitude.
template <class T>
double magnitude(cv::Point_<T> ab) {
    return sqrt(ab.x * ab.x + ab.y * ab.y);
}

// Calculate the distance from a to b.
template <class T>
double distance(cv::Point_<T> a, cv::Point_<T> b) {
    return magnitude<T>(b-a);
}

// wrap the index value so that it satisfies 0 >= index > points.size().  The index value will "wrap around" so that
// -1 will result in points.size()-1, and index=points.size() results in 0, etc.
template <class T>
uint wrap_index(std::vector<cv::Point_<T>> points, int index) {
    return (points.size() + index) % points.size();
}

// Returns the distance between the points at the given indices
template <class T>
double distance(std::vector<cv::Point_<T>> points, int index1, int index2) {
    return distance<T>(
            points[wrap_index<T>(points, index1)],
            points[wrap_index<T>(points, index2)]            
            );
}

// Calculate the distance from points[index] to p with index wrapping.
template <class T>
double distance(std::vector<cv::Point_<T>> points, int index, cv::Point_<T> p) {
    return distance<T>(points[wrap_index<T>(points, index)],p);
}

// Given points, a, b, and c, compute the angle at b (in degrees).
template <class T>
double compute_angle(cv::Point_<T> a, cv::Point_<T> b, cv::Point_<T> c) {
    
    cv::Point_<T> ab = a - b; // b - a;
    cv::Point_<T> bc = c - b;
    return 180.0 * acos(ab.ddot(bc) / (magnitude<T>(ab) * magnitude<T>(bc))) / M_PI;
}

// Computes the angle in degrees at the point with the specified index
template <class T>
double compute_angle(std::vector<cv::Point_<T>> points, int index) {
    return compute_angle<T>(
            points[wrap_index<T>(points, index - 1)],
            points[wrap_index<T>(points, index)],
            points[wrap_index<T>(points, index + 1)]
            );
}

// compute the angle in degrees at the point with the given index.  Uses index wrapping 
// to determine the point before and after the given index.
double compute_angle(std::vector<cv::Point> points, int index);

// draw a line from the point at index1 to the point at index2.  Applies index wrapping to the 
// specified indices.
void line(cv::Mat mat, std::vector<cv::Point> points, int index1, int index2, cv::Scalar color);

void filter(imlist to_filter, int size);
imlist color_to_bw(imlist color, int threshold);
void filter(imlist to_filter, int size);
imlist getImages(std::string path);
imlist blur(imlist to_blur, int size, double sigma);
imlist median_blur(imlist to_blur, int size);
imlist bilateral_blur(imlist to_blur);

template<class T> std::vector<cv::Point> translate_contour(std::vector<T> in , int offset_x, int offset_y);
std::vector<cv::Point> remove_duplicates(std::vector<cv::Point> vec);
//Return a contour that is translated
template<class T>
std::vector<cv::Point> translate_contour(std::vector<T> in , int offset_x, int offset_y){
    std::vector<cv::Point> ret_contour;
    cv::Point2f offset(offset_x,offset_y);
    for(uint i = 0; i<in.size(); i++){
        int x = (int)(in[i].x+offset_x+0.5);
        int y = (int)(in[i].y+offset_y+0.5);
        ret_contour.push_back(T(x,y));
    }
    return ret_contour;
}

void write_img(params& user_params, cv::Mat& img, std::string filename);
void write_debug_img(params& user_params, cv::Mat& img, std::string prefix, std::string index);
void write_debug_img(params& user_params, cv::Mat& img, std::string prefix, std::string index1, std::string index2);
void write_debug_img(params& user_params, cv::Mat& img, std::string prefix, uint index);
void write_debug_img(params& user_params, cv::Mat& img, std::string prefix, uint index1, uint index2);

#endif /* defined(__PuzzleSolver__Utils__) */
