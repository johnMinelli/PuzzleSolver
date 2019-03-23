//
//  puzzle.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/5/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__puzzle__
#define __PuzzleSolver__puzzle__

#include <iostream>
#include <string>
#include <vector>

#include "compat_opencv.h"

#include "edge.h"
#include "params.h"
#include "piece.h"
#include "PuzzleDisjointSet.h"


class puzzle{
private:
    struct match_score{
        uint16_t edge1, edge2;
        double score;
        static bool compare(match_score a, match_score b){
            return a.score<b.score;
        }
    };
    params& user_params;
    bool solved;
    std::vector<match_score> matches;
    std::vector<piece>  pieces;
    std::map<std::string,std::string> guided_matches;
    cv::Mat_<int> solution;
    cv::Mat_<int> solution_rotations;    
    std::vector<piece> extract_pieces();
    void print_edges();
    std::string edgeType_to_s(edgeType e);
    void auto_solve(PuzzleDisjointSet& p);
    void guided_solve(PuzzleDisjointSet& p);
public:
    puzzle(params& userParams);
    void load_guided_matches();
    bool guide_match(int p1, int e1, int p2, int e2);    
    void fill_costs();
    void solve();
    void save_solution_text();
    std::string get_solution_image_pathname();
    void save_solution_image();
    void show_solution_image();
};



#endif /* defined(__PuzzleSolver__puzzle__) */
