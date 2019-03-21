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
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include "omp.h"
#include "compat_opencv.h"

#include "PuzzleDisjointSet.h"
#include "utils.h"
#include "contours.h"
#include "logger.h"
#include "guided_match.h"

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
    pieces = extract_pieces();
    solved = false;
    if (user_params.isSavingEdges()) {
    	print_edges();
    }
    logger::stream() << "Extracted " << pieces.size() << " pieces" << std::endl;
    logger::flush();
}




void puzzle::print_edges(){
    
    cv::Scalar color = cv::Scalar(255);
    
    for(uint i =0; i<pieces.size(); i++){
        for(int j=0; j<4; j++){
            cv::Mat m = cv::Mat::zeros(500, 500, CV_8UC1 );

            std::vector<cv::Point> points = pieces[i].edges[j].get_translated_contour(200, 0);

            for (uint p = 0; p < points.size() -1 ; p++) {
                cv::line(m, points[p], points[p+1], color);
            }
            putText(m, pieces[i].edges[j].edge_type_to_s(), cv::Point(300,300),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, color, 1, COMPAT_CV_LINE_AA);

            write_debug_img(user_params, m, "edge", pieces[i].get_id(), std::to_string(j));
        }
    }
}


std::vector<piece> puzzle::extract_pieces() {
    std::vector<piece> pieces;
    imlist color_images = getImages(user_params.getInputDir());

    logger::stream() << "Extracting pieces..." << std::endl;    
    logger::flush();
    
    //Threshold the image, anything of intensity greater than 45 becomes white (255)
    //anything below becomes 0
//    imlist blured_images = blur(color_images, 7, 5);

    imlist bw;
    if(user_params.isUsingMedianFilter()){
        imlist blured_images = median_blur(color_images, user_params.getMedianBlurKSize());
        bw = color_to_bw(blured_images, user_params.getThreshold());
    } else{
        bw= color_to_bw(color_images, user_params.getThreshold());
        filter(bw,2);
    }

    uint unique_piece_id = user_params.getInitialPieceId();
    

    //For each input image
    for(uint i = 0; i<color_images.size(); i++){

        char image_number_buf[80];
        sprintf(image_number_buf, "%03d", i+1);
        std::string image_number(image_number_buf);
        
        if (user_params.isSavingOriginals()) {
            write_debug_img(user_params, bw[i],"original-bw", image_number);
            write_debug_img(user_params, color_images[i], "original-color", image_number);
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
        
        if (user_params.isSavingContours()) {
            std::vector<std::vector<cv::Point> > contours_to_draw;
            cv::Mat cmat = cv::Mat::zeros(bw[i].size().height, bw[i].size().width, CV_8UC3);    
            double font_scale = sqrt(bw[i].size().height * bw[i].size().width) / 1000;
            for (uint j = 0; j < contour_mgr.contours.size(); j++) {
                cv::Rect bounds = contour_mgr.contours[j].bounds;
                contours_to_draw.push_back(contour_mgr.contours[j].points);
                // Text indicating contour order within the image
                cv::putText(cmat, std::to_string(j+1), cv::Point2f(bounds.x+bounds.width/2-(10.0*font_scale),bounds.y+bounds.height/2+(10.0*font_scale)),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, font_scale, cv::Scalar(0, 255, 255), 1, COMPAT_CV_LINE_AA);                
            }

            cv::drawContours(cmat, contours_to_draw, -1, cv::Scalar(255,255,255), 2, 16);
            write_debug_img(user_params, cmat, "contours", image_number);
        }
        
        // Uncomment to save a version off the original with the piece numbers overlayed
        /*
        if (user_params.isSavingOriginals()) {
            cv::Mat cmat = color_images[i].clone();
            double font_scale = sqrt(bw[i].size().height * bw[i].size().width) / 1000;
            for (uint j = 0; j < contour_mgr.contours.size(); j++) {
                cv::Rect bounds = contour_mgr.contours[j].bounds;
                // Text indicating contour order within the image
                cv::putText(cmat, std::to_string(j+1), cv::Point2f(bounds.x+bounds.width/2-(10.0*font_scale),bounds.y+bounds.height/2+(10.0*font_scale)),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, font_scale, cv::Scalar(255,255,255), 2, cv::LINE_AA);                
            }

            write_debug_img(user_params, cmat, "numbered", image_number);
        }
        */
        
        for (uint j = 0; j < contour_mgr.contours.size(); j++) {
            int bordersize = 15;
            std::stringstream idstream;

            char id_buffer[80];
            snprintf(id_buffer, 80, "%03d-%03d-%04d", i+1, j+1, unique_piece_id);
            std::string piece_id(id_buffer);
            unique_piece_id += 1;
            
            cv::Rect bounds = contour_mgr.contours[j].bounds;
            std::vector<cv::Point> points = contour_mgr.contours[j].points;
            
            cv::Mat new_bw = cv::Mat::zeros(bounds.height+2*bordersize,bounds.width+2*bordersize,CV_8UC1);
            std::vector<std::vector<cv::Point> > contours_to_draw;
            contours_to_draw.push_back(translate_contour(points, bordersize-bounds.x, bordersize-bounds.y));
            cv::drawContours(new_bw, contours_to_draw, -1, cv::Scalar(255), COMPAT_CV_FILLED);

            if (user_params.isSavingBlackWhite()) {
                write_debug_img(user_params, new_bw, "bw", piece_id);
            }

            cv::Rect b2(bounds.x-3, bounds.y-3, bounds.width+6, bounds.height+6);
            cv::Mat color_roi = color_images[i](b2);
            cv::Mat mini_color = cv::Mat::zeros(bounds.height+2*bordersize,bounds.width+2*bordersize,CV_8UC3);
            color_roi.copyTo(mini_color(cv::Rect(bordersize-3,bordersize-3,b2.width,b2.height)));
            
            if (user_params.isSavingColor()) {
                write_debug_img(user_params, mini_color, "color", piece_id);
            }
            cv::Mat mini_bw = new_bw;
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
            score.score = pieces[i/4].edges[i%4].compare3(pieces[j/4].edges[j%4]);
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
    
    load_guided_matches();
    
    logger::stream() << "Finding edge costs..." << std::endl;
    logger::flush();
    fill_costs();
    std::vector<match_score>::iterator i= matches.begin();
    PuzzleDisjointSet p(this, user_params, (int)pieces.size());
    
    
    int output_id=0;
    while(!p.in_one_set() && i!=matches.end() ){
        int p1 = i->edge1/4;
        int e1 = i->edge1%4;
        int p2 = i->edge2/4;
        int e2 = i->edge2%4;
        
        if (user_params.isSavingMatches()) {
            cv::Mat m = cv::Mat::zeros(500,500,CV_8UC1);
            std::stringstream out_file_name;
            out_file_name << user_params.getOutputDir() << "match" << output_id << "_" << pieces[p1].get_id() << "-" << e1 << "_" << pieces[p2].get_id() << "-" <<e2 << ".png";
            std::vector<std::vector<cv::Point> > contours;
            contours.push_back(pieces[p1].edges[e1].get_translated_contour(200, 0));
            contours.push_back(pieces[p2].edges[e2].get_translated_contour_reverse(200, 0));
            cv::polylines(m, contours, false, cv::Scalar(255));
            cv::imwrite(out_file_name.str(), m);
        }
        if (user_params.isVerbose()) {
            logger::stream() << "Attempting to merge: " << pieces[p1].get_id() << "-" << (e1+1) << " with: " << 
                    pieces[p2].get_id() << "-" << (e2+1) << ", score:" << i->score << " count: "  << output_id <<std::endl;
            logger::flush();
        }
        p.join_sets(p1, p2, e1, e2);
        i++;
        output_id += 1;
    }
    
    p.finish();
    
    if(p.in_one_set()){
        logger::stream() << "Possible solution found" << std::endl;
        logger::flush();
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

std::string get_guided_matches_filename(params& user_params) {
    std::stringstream fnstream;
    fnstream << user_params.getOutputDir() << "guided-matches.dat";
    return fnstream.str();    
}

void puzzle::load_guided_matches() {
    std::string filename = get_guided_matches_filename(user_params);
    std::ifstream istream;
    istream.open(filename, std::ifstream::in);
    if (istream.fail()) {
        return;
    }

    std::string dateField;
    std::string isMatchField;
    std::string piecePairId;
    
    while (true) {

        istream >> dateField;
        istream >> isMatchField;
        istream >> piecePairId;
        if (istream.eof()) {
            break;
        }
        
        guided_matches[piecePairId] = isMatchField;
    }
    
    istream.close();
}

// Returns a string identifying the piece-edge paring
std::string get_match_id(int initial_piece_id, int p1, int p2, int e1, int e2) {
    // Create the ID with the lower value piece number appearing first.
    int id_p1;
    int id_e1;
    int id_p2;
    int id_e2;
    
    if (p1 < p2) {
        id_p1 = p1 + initial_piece_id;
        id_e1 = (e1+1);
        id_p2 = p2 + initial_piece_id;
        id_e2 = (e2+1);
    } else {
        id_p1 = p2 + initial_piece_id; 
        id_e1 = (e2+1);
        id_p2 = p1 + initial_piece_id;
        id_e2 = (e1+1);                
    }

    std::stringstream idstream;
    idstream << id_p1 << "-" << id_e1 << "-" << id_p2 << "-" << id_e2;
    return idstream.str();    
}

bool puzzle::guide_match(int p1, int p2, int e1, int e2) {
    
    std::string id = get_match_id(user_params.getInitialPieceId(), p1, p2, e1, e2);
    
    std::map<std::string,std::string>::iterator it = guided_matches.find(id);
    if (it != guided_matches.end()) {
        return (it->second == "yes");
    }
    
    if (!user_params.isGuidedSolution()) {
        return true;  // true results in the default automatic solution behavior
    }
    
    std::cout << "Does piece " << (p1 + user_params.getInitialPieceId()) << " fit to " << (p2 + user_params.getInitialPieceId()) << " ?" << std::flush;
    
    std::string response = guided_match(pieces[p1], pieces[p2], e1, e2, user_params);
    std::cout << "Guided match " << response << std::endl;
    
    bool match = (response == "yes");

    std::string filename = get_guided_matches_filename(user_params);
    std::ofstream ostream;
    ostream.open(filename, std::ofstream::out | std::ofstream::app);
    if (ostream.fail()) {
        std::cerr << "Failed to open " << filename << " for writing" << std::endl;
        exit(1);
    }
    
    std::time_t time = std::time(NULL);
    std::tm tm = *std::localtime(&time);
    ostream << std::put_time(&tm, "%a_%F_%T") << (match ? " yes " : "  no ") << id << "\n" << std::flush;
    ostream.close();
    return match;
}

// Generates and displays the solution as text (grid of piece IDs).  The text is also saved to 
// <solution>.txt in the output directory.  The numbers are 1-based instead of zero-based so
// that they correspond to the piece IDs.
void puzzle::save_solution_text() {
    if(!solved) solve();
    
    std::stringstream stream;
    
    int width = 2;
    int max_id = pieces.size() + user_params.getInitialPieceId() -1;
    if (max_id > 99) {
        width = 3;
    } 
    else if (max_id > 999) {
        width = 4;
    }
    
    for(int row = 0; row < solution.rows; ++row) {
        int* p = (int*)solution.ptr(row);
        for(int col = 0; col < solution.cols; ++col) {
            int value = *p++;
            if (value >= 0) {
                stream << std::setw(width) << (value + user_params.getInitialPieceId());
            }
            else {
                stream << std::setw(width) << "";
            }
            stream << ", ";
        }
        stream << std::endl;
    }
    
    stream << std::endl;
    
    std::string solution_text = stream.str();
    logger::stream() << solution_text << std::endl;
    logger::flush();
      
}

std::string puzzle::get_solution_image_pathname() {
    return user_params.getOutputDir() + user_params.getSolutionFileBasename() + ".png";
}


/**
 * Function to check if the color of the given image
 * is the same as the given color
 *
 * Parameters:
 *   edge        The source image
 *   color   The color to check
 */
bool is_border(cv::Mat& edge, cv::Vec3b color)
{
    cv::Mat im = edge.clone().reshape(0,1);

    bool res = true;
    for (int i = 0; i < im.cols; ++i)
        res &= (color == im.at<cv::Vec3b>(0,i));

    return res;
}

/**
 * Function to auto-cropping image
 *
 * Parameters:
 *   src   The source image
 *   dst   The destination image
 */
void autocrop(cv::Mat& src, cv::Mat& dst)
{
    cv::Rect win(0, 0, src.cols, src.rows);

    std::vector<cv::Rect> edges;
    edges.push_back(cv::Rect(0, 0, src.cols, 1));
    edges.push_back(cv::Rect(src.cols-2, 0, 1, src.rows));
    edges.push_back(cv::Rect(0, src.rows-2, src.cols, 1));
    edges.push_back(cv::Rect(0, 0, 1, src.rows));

    cv::Mat edge;
    int nborder = 0;
    cv::Vec3b color = src.at<cv::Vec3b>(src.cols-1,src.rows-1);

    for (int i = 0; i < edges.size(); ++i)
    {
        edge = src(edges[i]);
        nborder += is_border(edge, color);
    }

    if (nborder == 0)
    {
        src.copyTo(dst);
        return;
    }

    bool next;

    do {
        edge = src(cv::Rect(win.x, win.height-2, win.width, 1));
        if ((next = is_border(edge, color)))
            win.height--;
    }
    while (next && win.height > 0);

    do {
        edge = src(cv::Rect(win.width-2, win.y, 1, win.height));
        if ((next = is_border(edge, color)))
            win.width--;
    }
    while (next && win.width > 0);

    do {
        edge = src(cv::Rect(win.x, win.y, win.width, 1));
        if ((next = is_border(edge, color)))
            win.y++, win.height--;
    }
    while (next && win.y <= src.rows);

    do {
        edge = src(cv::Rect(win.x, win.y, 1, win.height));
        if ((next = is_border(edge, color)))
            win.x++, win.width--;
    }
    while (next && win.x <= src.cols);

    dst = src(win);
}

//Saves an image of the representation of the puzzle.
//only really works when there are no holes
//TODO: fail when puzzle is in configurations that are not possible i.e. holes
void puzzle::save_solution_image(){
    if(!solved) solve();
    
    
    //Use get affine to map points...
    int out_image_size = 6000;
    cv::Mat out_image(out_image_size,out_image_size,CV_8UC3, cv::Scalar(200,50,3));
    int border = 10;
    
    cv::Point2f ** points = new cv::Point2f*[solution.size[0]+1];
    for(int i = 0; i < solution.size[0]+1; ++i)
        points[i] = new cv::Point2f[solution.size[1]+1];
    bool failed=false;
    
    logger::stream() << "Saving image..." << std::endl;
    logger::flush();
    for(int i=0; i<solution.size[0];i++){
        for(int j=0; j<solution.size[1]; j++){
            int piece_number = solution(i,j);
            logger::stream() << std::setfill(' ') << std::setw(2) << "." << std::flush; logger::flush();

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
            
            layer.copyTo(out_image(cv::Rect(0,0,layer_size,layer_size)), layer_mask);
            
        }
        logger::stream() << std::endl; logger::flush();

    }
    if(failed){
        logger::stream() << "Failed, only partial image generated" << std::endl; logger::flush();
    }

    cv::Mat final_out_image;
    
    autocrop(out_image, final_out_image);
    cv::imwrite(get_solution_image_pathname(), final_out_image);
    
    

    for(int i = 0; i < solution.size[0]+1; ++i)
        delete points[i];
    delete[] points;
    
}

#if OPENCV_VERSION_MAJOR == 2
// Mimic cv::rotate(src,dest,rotation_code) which is available starting in OpenCV 3.x 
void cv_rotate(const cv::Mat& image, cv::Mat& dest, int rotation_code)
{
  switch (rotation_code) {
  case ROTATE_90_CLOCKWISE:
    cv::flip(image.t(), dest, 1);
    break;
  case ROTATE_180:
    cv::flip(image, dest, -1);
    break;
  case ROTATE_90_COUNTERCLOCKWISE:
    cv::flip(image.t(), dest, 0);
    break;
  default:
    dest = image.clone();
    break;
  }
}
#endif

// Initial width, the window is resizable
#define SOLUTION_WINDOW_WIDTH 768

void puzzle::show_solution_image() {
    cv::Mat solution_image = cv::imread(get_solution_image_pathname());

    float aspect = (float)solution_image.size().width / solution_image.size().height;
    
    int height = SOLUTION_WINDOW_WIDTH / aspect;
    
    std::string window_name = "solution";
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(window_name, SOLUTION_WINDOW_WIDTH, height);

    int rotation_code = 3; // 0 = 90, 1 = 180, 2 = 270, 3 = 0
    cv::Mat rotated;
    cv::imshow(window_name, solution_image);
    
    std::cout << "With focus on the solution image window:" << std::endl;
    std::cout << "    press the 'r' key one or more times to rotate the solution image by 90 degrees" << std::endl;
    std::cout << "    press the 'q' to quit/exit" << std::endl;
    
    bool done = false;
    do {
        int key = cv::waitKey(0);    
        switch (key) {
            case -1:
            case 'q':
                done = true;
                break;
            case 'r':
                rotation_code = (rotation_code + 1) % 4;
                if (rotation_code == 3) {
                    rotated = solution_image.clone();
                } else {
                    compat_cv_rotate(solution_image, rotated, rotation_code);
                }
                cv::imshow(window_name, rotated);
                break;
            default:
                break;
        }
    } while (!done);
    try {
        cv::destroyWindow(window_name);
    }
    catch (cv::Exception x) {
        // Ignore
    }
}
