//
//  PuzzleDisjointSet.h
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/12/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#ifndef __PuzzleSolver__PuzzleDisjointSet__
#define __PuzzleSolver__PuzzleDisjointSet__

#include <iostream>
#include <vector>
#include "compat_opencv.h"
#include "params.h"

class PuzzleDisjointSet{
public:
    struct forest{
        cv::Mat_<int> locations;
        cv::Mat_<int> rotations;
        int representative;
        int id;
    };
    struct join_context {
        bool joinable;
        int rep_a;
        int rep_b;
        cv::Mat_<int> new_a_locs;
        cv::Mat_<int> new_a_rots;        
    };
private:
    //A count of how many sets are left.
    int set_count;
    uint merge_failures;
    std::vector<forest> sets;
    std::vector<int> csets; // collector sets... matched sets that are currently unmatched with any other set
    params& user_params;
    void rotate_ccw(int id, int times);
    void make_set(int x);
    cv::Point find_location(cv::Mat_<int>, int number );
public:
    PuzzleDisjointSet(params& user_params, int number);
    join_context& compute_join(int a, int b, int how_a, int how_b);
    void complete_join(join_context& context);
//    bool join_sets(int a, int b, int how_a, int how_b);
    int find(int a);
    // returns true if the set is a matched set that is unmatched with any other sets
    bool is_collection_set(int rep);
    // returns true of the set is unmatched
    bool is_unmatched_set(int rep);
    int collection_set_count();
    bool in_same_set(int a, int b);
    bool in_one_set();
    forest get(int id);
    void finish();    
};





#endif /* defined(__PuzzleSolver__PuzzleDisjointSet__) */
