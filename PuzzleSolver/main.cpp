//
//  main.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/4/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <string.h>
#include <cassert>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <libgen.h>

#include "cxxopts.hpp"
#include "params.h"
#include "puzzle.h"
#include "PuzzleDisjointSet.h"
#include "utils.h"
#include "contours.h"


class demo {
public:
    std::string name;
    std::string inputDir;
    int estimated_piece_size;
    int threshold;
    bool filter;
    std::string comment;

    demo(std::string name, std::string inputDir, int estimated_piece_size, int threshold, bool filter, std::string comment) :
        name(name), inputDir(inputDir), estimated_piece_size(estimated_piece_size), 
        threshold(threshold), filter(filter), comment(comment) 
    {
    }

};

void register_demo(std::map<std::string,demo*> &demos, std::string name, std::string inputDir, int estimated_piece_size, int threshold, bool filter, std::string comment)
{
    demo* demoptr = new demo(name, inputDir, estimated_piece_size, threshold, filter, comment);
    demos[demoptr->name] = demoptr;
}

void demo_help(std::map<std::string,demo*> &demos)
{
    std::cout << std::setw(24) << std::left << "DEMO NAME" << "   COMMENT" << std::endl;
    for (std::map<std::string,demo*>::iterator it=demos.begin(); it!=demos.end(); ++it) {
        demo* d = it->second;
        std::cout << std::setw(24) << std::left << d->name << " : " << d->comment << std::endl;
    }
}
int main(int argc, char * argv[])
{
    params user_params;
    
    std::map<std::string,demo*> demos;
    register_demo(demos, "toy-story-color", "Toy Story", 200, 22, true, "48 pieces, estimated-size=200, threshold=22, filter()");
    register_demo(demos, "toy-story-back", "Toy Story back", 200, 50, false, "48 pieces, estimated-size=200, threshold=50, median_filter()");
    register_demo(demos, "angry-birds-color", "Angry Birds/color", 300, 30, false, "24 pieces, estimated-size=300, threshold=30, median_filter()");
    register_demo(demos, "angry-birds-scanner-open", "Angry Birds/Scanner Open", 300, 30, false, "24 pieces, estimated-size=300, threshold=30, median_filter()");
    register_demo(demos, "horses", "horses", 380, 50, false, "104 pieces, estimated-size=380, threshold=50, median_filter()");
    register_demo(demos, "horses-numbered", "horses numbered", 380, 50, false, "104 pieces, estimated-size=380, threshold=50, median_filter()");

    cxxopts::Options options("PuzzleSolver", "Solve jigsaw puzzles using the shapes of the piece edges");

    options.add_options()
      ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
      ("h,help", "Display this help message")
      ("s,solve", "Solve the puzzle after processing the input images and extracting pieces and edges", cxxopts::value<bool>()->default_value("false"))
      ("dont-solve", "Skip finding the solution (e.g., for a demo which normally implies --solve)", cxxopts::value<bool>()->default_value("false"))
      ("n,solution-name", "Basename for solution text/image files written to the output directory", cxxopts::value<std::string>()->default_value("solution"))      
      ("e,estimated-size", "Estimated piece size", cxxopts::value<int>()->default_value("200"))
      ("t,threshold", "Threshold value used when converting color images to b&w.  Min: 0, max: 255.", cxxopts::value<int>()->default_value("30"))
      ("f,filter", "Use filter() instead of median_filter()", cxxopts::value<bool>()->default_value("false"))
      ("o,order", "Order of pieces in the input images", cxxopts::value<std::string>()->default_value("lrtb"))
      ("p,partition", "Piece-ordering partition factor for adjusting behavior of --order", cxxopts::value<float>()->default_value("1.0"))                
      ("b,corners-blocksize", "Block size to use when finding corners", cxxopts::value<int>()->default_value("25"))            
      ("c,corners-quality", "Corner quality warning threshold", cxxopts::value<int>()->default_value("300"))              
      ("g,edit-corners","Show GUI corner editor for each piece where its corner quality exceeds the corners quality threshold", cxxopts::value<bool>()->default_value("false"))
      ("corner-edit-scale","Scale factor for images shown in the corner editor",  cxxopts::value<float>()->default_value("1.0"))
      ("save-all", "Save all images (originals, contours, b&w, color, corners, edges)", cxxopts::value<bool>()->default_value("false"))
      ("save-originals", "Save original images", cxxopts::value<bool>()->default_value("false"))                        
      ("save-contours", "Save contour images", cxxopts::value<bool>()->default_value("false"))            
      ("save-bw", "Save black&white piece images", cxxopts::value<bool>()->default_value("false"))                        
      ("save-color", "Save color piece images", cxxopts::value<bool>()->default_value("false"))                                    
      ("save-corners", "Save piece images showing corner locations", cxxopts::value<bool>()->default_value("false"))                                                
      ("save-edges", "Save images for each piece edge", cxxopts::value<bool>()->default_value("false")) 
      ("d,demo","Solve a named demo puzzle.  See below for a list of demos.", cxxopts::value<std::string>()) 

      ("positional","Positional parameters", cxxopts::value<std::vector<std::string>>())
      ;

    options.parse_positional({"positional"});
    options.positional_help("<directory containing input images> <output directory>");
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
    	std::cout << options.help({"", "Group"}) << std::endl;
        demo_help(demos);        
    	exit(0);
    }
    
    bool is_demo = result.count("demo") > 0;
    if (is_demo) {
        std::string demo_name = result["demo"].as<std::string>();
        std::map<std::string,demo*>::iterator it = demos.find(demo_name);
        if (it == demos.end()) {
            std::cout << "ERROR: Unknown demo name: " << demo_name << std::endl;
            std::cout << "Use --help to get the list of demo names" << std::endl;            
            exit(1);
        }
        demo* demoptr = it->second;  

        // Resolve the PuzzleSolver home dir (parent of 'Scans'), by assuming the PuzzleSolver exe file is in the source directory.
        char dirnamebuf[300];
        realpath(argv[0], dirnamebuf);
        char* puzzleSolverHome=dirname(dirname(dirnamebuf));
        
        user_params.setInputDir(std::string(puzzleSolverHome) + "/Scans/" + demoptr->inputDir);
        user_params.setOutputDir("/tmp/"+demoptr->name);
        user_params.setSolving(true);        
        user_params.setEstimatedPieceSize(demoptr->estimated_piece_size);
        user_params.setThreshold(demoptr->threshold);
        user_params.setUsingMedianFilter(!demoptr->filter);

        // Allow some demo default values to be explicity overridden
        if (result.count("estimated-size")) {
            user_params.setEstimatedPieceSize(result["estimated-size"].as<int>());            
        }
        if (result.count("threshold")) {
            user_params.setThreshold(result["threshold"].as<int>());
        }
        if (result.count("filter")) {
            user_params.setUsingMedianFilter(!result["filter"].as<bool>());
        }
    }
    else {
        if (result.count("positional") != 2)
        {
            std::cout << "ERROR: check positional args" << std::endl << std::endl;
            std::cout << options.help({"", "Group"}) << std::endl;
            exit(1);
        }
        auto& positional = result["positional"].as<std::vector<std::string>>();
        user_params.setInputDir(positional[0]);
        user_params.setOutputDir(positional[1]);
        user_params.setSolving(result["solve"].as<bool>());
        user_params.setEstimatedPieceSize(result["estimated-size"].as<int>());
        user_params.setThreshold(result["threshold"].as<int>());
        user_params.setUsingMedianFilter(!result["filter"].as<bool>());        
    }
    
    std::string order = result["order"].as<std::string>();
    if (piece_order::lookup(order) == NULL) {
        std::cout << "ERROR: Order '" << order << "' is invalid, expected one of: ";
        std::vector<std::string> nv;
        piece_order::names(nv);
        for (int i = 0; i < nv.size(); i++) {
            std::cout << nv[i];
            if (i < (nv.size() - 1)) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
        
        exit(1);
    }
    
    if (result.count("dont-solve")) {
        user_params.setSolving(false);
    }
    user_params.setVerbose(result["verbose"].as<bool>());
    user_params.setSolutionFileBasename(result["solution-name"].as<std::string>());
    user_params.setPieceOrder(result["order"].as<std::string>());
    user_params.setPartitionFactor(result["partition"].as<float>());
    user_params.setFindCornersBlockSize(result["corners-blocksize"].as<int>());
    user_params.setMinCornersQuality(result["corners-quality"].as<int>());  
    user_params.setEditingCorners(result["edit-corners"].as<bool>());
    user_params.setCornerEditorScale(result["corner-edit-scale"].as<float>());
    user_params.setSaveAll(result["save-all"].as<bool>());
    user_params.setSavingOriginals(result["save-originals"].as<bool>());    
    user_params.setSavingContours(result["save-contours"].as<bool>());        
    user_params.setSavingBlackWhite(result["save-bw"].as<bool>());    
    user_params.setSavingColor(result["save-color"].as<bool>());    
    user_params.setSavingCorners(result["save-corners"].as<bool>());    
    user_params.setSavingEdges(result["save-edges"].as<bool>());        
    user_params.show();

    mkdir(user_params.getOutputDir().c_str(), 0775);


    std::cout << "Starting..." << std::endl;
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    long inbetween_millis = millis;


    puzzle puzzle(user_params);

    gettimeofday(&time, NULL);
    std::cout << std::endl << "time to initialize:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    if (!user_params.isSolving()) {
        return 0;
    }
    
    puzzle.solve();
    gettimeofday(&time, NULL);
    std::cout << std::endl << "time to solve:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    puzzle.save_solution_text();
    puzzle.save_solution_image();
    gettimeofday(&time, NULL);
    std::cout << std::endl << "Time to draw:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    
    
    gettimeofday(&time, NULL);
    std::cout << std::endl << "total time:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;
    
    puzzle.show_solution_image();
    
    return 0;
}


