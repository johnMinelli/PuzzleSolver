//
//  puzzle.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include "puzzle.h"

#include <sstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <stdio.h>
#include "omp.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/video/tracking.hpp"

#include "PuzzleDisjointSet.h"
#include "utils.h"
#include "contours.h"

typedef std::vector<cv::Mat> imlist;

/*
                   _________      _____
                   \        \    /    /
                    |       /    \   /   _
                 ___/       \____/   |__/ \
                /       PUZZLE SOLVER      }
                \__/\  JOE ___ ZEIMEN  ___/
                     \    /   /       /
                     |    |  |       |
                    /_____/   \_______\
*/



puzzle::puzzle(params& _user_params) : user_params(_user_params) {
    std::cout << "extracting pieces" << std::endl;
    pieces = extract_pieces();
    solved = false;
    if (user_params.isGeneratingDebugOutput()) {
    	print_edges();
    }
}




void puzzle::print_edges(){
    for(uint i =0; i<pieces.size(); i++){
        for(int j=0; j<4; j++){
            cv::Mat m = cv::Mat::zeros(500, 500, CV_8UC1 );

            std::vector<std::vector<cv::Point> > contours;
            contours.push_back(pieces[i].edges[j].get_translated_contour(200, 0));
            //This isn't used but the opencv function wants it anyways.
            std::vector<cv::Vec4i> hierarchy;

            cv::drawContours(m, contours, -1, cv::Scalar(255));

            putText(m, pieces[i].edges[j].edge_type_to_s(), cvPoint(300,300),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255), 1, CV_AA);

            write_debug_img(user_params, m, "edge", i, j);
        }
    }
}

// value at index
template <class T>
class vai {
public:
    int index;
    T value;
    vai(int _index, T _value) : index(_index), value(_value) {}
};

std::vector<piece> puzzle::extract_pieces() {
    std::vector<piece> pieces;
    imlist color_images = getImages(user_params.getInputDir());

    //Threshold the image, anything of intensity greater than 45 becomes white (255)
    //anything below becomes 0
//    imlist blured_images = blur(color_images, 7, 5);

    imlist bw;
    if(user_params.isUsingMedianFilter()){
        imlist blured_images = median_blur(color_images, 5);
        bw = color_to_bw(blured_images, user_params.getThreshold());
    } else{
        bw= color_to_bw(color_images, user_params.getThreshold());
        filter(bw,2);
    }

    uint piece_count = 0;
    

    //For each input image
    for(uint i = 0; i<color_images.size(); i++){

        if (user_params.isGeneratingDebugOutput()) {
            write_debug_img(user_params, bw[i],"bw", i);
            write_debug_img(user_params, color_images[i], "color", i);
        }

        std::vector<std::vector<cv::Point> > found_contours;

        
        //This isn't used but the opencv function wants it anyways.
        std::vector<cv::Vec4i> hierarchy;

        //Need to clone b/c it will get modified
        cv::findContours(bw[i].clone(), found_contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        
 
        //For each contour in that image
        //TODO: (In anticipation of the other TODO's Re-create the b/w image
        //    based off of the contour to eliminate noise in the layer mask

        contour_mgr contour_mgr(bw[i].size().width, bw[i].size().height, user_params); 

        for(uint j = 0; j < found_contours.size(); j++) {
            cv::Rect bounds =  cv::boundingRect(found_contours[j]);
            if(bounds.width < user_params.getEstimatedPieceSize() || bounds.height < user_params.getEstimatedPieceSize()) continue;
            
            contour_mgr.add_contour(bounds, remove_duplicates(found_contours[j]));
        }

        contour_mgr.sort_contours();
        

        for (uint j = 0; j < contour_mgr.contours.size(); j++) {
            int bordersize = 15;
            std::stringstream idstream;

            piece_count += 1;
            char id_buffer[80];

            sprintf(id_buffer, "%03d-%03d-%04d", i+1, j+1, piece_count);
            std::string piece_id(id_buffer);
            
            cv::Rect bounds = contour_mgr.contours[j].bounds;
            std::vector<cv::Point> points = contour_mgr.contours[j].points;
            
            cv::Mat new_bw = cv::Mat::zeros(bounds.height+2*bordersize,bounds.width+2*bordersize,CV_8UC1);
            std::vector<std::vector<cv::Point> > contours_to_draw;
            contours_to_draw.push_back(translate_contour(points, bordersize-bounds.x, bordersize-bounds.y));
            cv::drawContours(new_bw, contours_to_draw, -1, cv::Scalar(255), CV_FILLED);

            if (user_params.isGeneratingDebugOutput()) {
                write_debug_img(user_params, new_bw, "contour", piece_id);
            }

            bounds.width += bordersize*2;
            bounds.height += bordersize*2;
            bounds.x -= bordersize;
            bounds.y -= bordersize;
//            cv::imwrite("/tmp/final/bw.png", bw[i](bounds));
            cv::Mat mini_color = color_images[i](bounds);
            cv::Mat mini_bw = new_bw;//bw[i](bounds);
            //Create a copy so it can't conflict.
            mini_color = mini_color.clone();
            mini_bw = mini_bw.clone();
            
            piece p(piece_id, mini_color, mini_bw, user_params);
            pieces.push_back(p);
            
        }
    }
    
    return pieces;
}




void puzzle::fill_costs(){
    int no_edges = (int) pieces.size()*4;
    
    //TODO: use openmp to speed up this loop w/o blocking the commented lines below
//    omp_set_num_threads(4);
#pragma omp parallel for schedule(dynamic)
    for(int i =0; i<no_edges; i++){
        for(int j=i; j<no_edges; j++){
            match_score score;
            score.edge1 =(int) i;
            score.edge2 =(int) j;
            score.score = pieces[i/4].edges[i%4].compare2(pieces[j/4].edges[j%4]);
#pragma omp critical
{
            matches.push_back(score);
}
        }
    }
    std::sort(matches.begin(),matches.end(),match_score::compare);
}



//Solves the puzzle
void puzzle::solve(){
    
    std::cout << "Finding edge costs..." << std::endl;
    fill_costs();
    std::vector<match_score>::iterator i= matches.begin();
    PuzzleDisjointSet p((int)pieces.size());
    
  
//You can save the individual pieces with their id numbers in the file name
//If the following loop is uncommented.
//    for(int i=0; i<pieces.size(); i++){
//        std::stringstream filename;
//        filename << "/tmp/final/p" << i << ".png";
//        cv::imwrite(filename.str(), pieces[i].full_color);
//    }
    
//    int output_id=0;
    while(!p.in_one_set() && i!=matches.end() ){
        int p1 = i->edge1/4;
        int e1 = i->edge1%4;
        int p2 = i->edge2/4;
        int e2 = i->edge2%4;
        
//Uncomment the following lines to spit out pictures of the matched edges...
//        cv::Mat m = cv::Mat::zeros(500,500,CV_8UC1);
//        std::stringstream out_file_name;
//        out_file_name << "/tmp/final/match" << output_id++ << "_" << p1<< "_" << e1 << "_" <<p2 << "_" <<e2 << ".png";
//        std::vector<std::vector<cv::Point> > contours;
//        contours.push_back(pieces[p1].edges[e1].get_translated_contour(200, 0));
//        contours.push_back(pieces[p2].edges[e2].get_translated_contour_reverse(200, 0));
//        cv::drawContours(m, contours, -1, cv::Scalar(255));
//        std::cout << out_file_name.str() << std::endl;
//        cv::imwrite(out_file_name.str(), m);
//        std::cout << "Attempting to merge: " << p1 << " with: " << p2 << " using edges:" << e1 << ", " << e2 << " c:" << i->score << " count: "  << output_id++ <<std::endl;
        p.join_sets(p1, p2, e1, e2);
        i++;
    }
    
    p.finish();
    
    if(p.in_one_set()){
        std::cout << "Possible solution found" << std::endl;
        solved = true;
        solution = p.get(p.find(1)).locations;
        solution_rotations = p.get(p.find(1)).rotations;
        
        for(int i =0; i<solution.size[0]; i++){
            for(int j=0; j<solution.size[1]; j++){
                int piece_number = solution(i,j);
                pieces[piece_number].rotate(4-solution_rotations(i,j));
            }
        }
        
        
    }
    
    
    
}



//Saves an image of the representation of the puzzle.
//only really works when there are no holes
//TODO: fail when puzzle is in configurations that are not possible i.e. holes
void puzzle::save_image(){
    if(!solved) solve();
    
    std::cout << solution << std::endl;
    
    //Use get affine to map points...
    int out_image_size = 6000;
    cv::Mat final_out_image(out_image_size,out_image_size,CV_8UC3, cv::Scalar(200,50,3));
    int border = 10;
    
    cv::Point2f ** points = new cv::Point2f*[solution.size[0]+1];
    for(int i = 0; i < solution.size[0]+1; ++i)
        points[i] = new cv::Point2f[solution.size[1]+1];
    bool failed=false;
    
    std::cout << "Saving image..." << std::endl;
    for(int i=0; i<solution.size[0];i++){
        for(int j=0; j<solution.size[1]; j++){
            int piece_number = solution(i,j);
            std::cout << solution(i,j) << ",";

            if(piece_number ==-1){
                failed = true;
                // break;
                continue;
            }
            float x_dist =(float) cv::norm(pieces[piece_number].get_corner(0)-pieces[piece_number].get_corner(3));
            float y_dist =(float) cv::norm(pieces[piece_number].get_corner(0)-pieces[piece_number].get_corner(1));
            std::vector<cv::Point2f> src;
            std::vector<cv::Point2f> dst;
            
            if(i==0 && j==0){
                points[i][j] = cv::Point2f(border,border);
            }
            if(i==0){
                points[i][j+1] = cv::Point2f(points[i][j].x+border+x_dist,border);
            }
            if(j==0){
                points[i+1][j] = cv::Point2f(border,points[i][j].y+border+y_dist);
            }
            
            dst.push_back(points[i][j]);
            dst.push_back(points[i+1][j]);
            dst.push_back(points[i][j+1]);
            src.push_back(pieces[piece_number].get_corner(0));
            src.push_back(pieces[piece_number].get_corner(1));
            src.push_back(pieces[piece_number].get_corner(3));

            //true means use affine transform
            cv::Mat a_trans_mat = cv::estimateRigidTransform(src, dst,true);
            cv::Mat_<double> A = a_trans_mat;
            
            //Lower right corner of each piece
            cv::Point2f l_r_c = pieces[piece_number].get_corner(2);
            
            //Doing my own matrix multiplication
            points[i+1][j+1] = cv::Point2f((float)(A(0,0)*l_r_c.x+A(0,1)*l_r_c.y+A(0,2)),(float)(A(1,0)*l_r_c.x+A(1,1)*l_r_c.y+A(1,2)));
            
            
            
            cv::Mat layer;
            cv::Mat layer_mask;
            
            int layer_size = out_image_size;
            
            cv::warpAffine(pieces[piece_number].full_color, layer, a_trans_mat, cv::Size2i(layer_size,layer_size),cv::INTER_LINEAR,cv::BORDER_TRANSPARENT);
            cv::warpAffine(pieces[piece_number].bw, layer_mask, a_trans_mat, cv::Size2i(layer_size,layer_size),cv::INTER_NEAREST,cv::BORDER_TRANSPARENT);
            
            layer.copyTo(final_out_image(cv::Rect(0,0,layer_size,layer_size)), layer_mask);
            
        }
        std::cout << std::endl;

    }
    if(failed){
        std::cout << "Failed, only partial image generated" << std::endl;
    }

    cv::imwrite(user_params.getOutputDir() + user_params.getSolutionImageFilename(),final_out_image);
    
    

    for(int i = 0; i < solution.size[0]+1; ++i)
        delete points[i];
    delete[] points;
    
}
