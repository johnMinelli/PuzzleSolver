//
//  main.cpp
//  PuzzleSolver
//
//  Created by Joe Zeimen on 4/4/13.
//  Copyright (c) 2013 Joe Zeimen. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <cassert>
#include <sys/time.h>
#include <sys/stat.h>

#include "cxxopts.hpp"
#include "params.h"
#include "puzzle.h"
#include "PuzzleDisjointSet.h"
#include "utils.h"

//Dont forget final "/" in directory name.
// static const std::string input = "/Users/jzeimen/Documents/school/College/Spring2013/ComputerVision/FinalProject/PuzzleSolver/PuzzleSolver/Scans/";
// static const std::string output = "/tmp/final/finaloutput.png";


int main(int argc, char * argv[])
{
    
	cxxopts::Options options("PuzzleSolver", "Solve jigsaw puzzles using the shapes of the piece edges");

	options.add_options()
          ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
	  ("h,help", "Display this help message")
	  ("s,size", "Estimated piece size", cxxopts::value<int>()->default_value("200"))
	  ("t,threshold", "Threshold", cxxopts::value<int>()->default_value("30"))
	  ("f,filter", "Use filter() instead of median_filter()", cxxopts::value<bool>()->default_value("false"))
	  ("d,debug-dir", "Save intermediate output files to this directory", cxxopts::value<std::string>())
	  ("l,landscape", "Input images are in landscape orientation", cxxopts::value<bool>()->default_value("false"))                
          ("p,partition", "Piece partition factor", cxxopts::value<float>()->default_value("1.0"))                
	  ("positional","Positional parameters", cxxopts::value<std::vector<std::string>>())
	  ;

	options.parse_positional({"positional"});
	options.positional_help("<input images directory> <output image name>");
	auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
    	std::cout << options.help({"", "Group"}) << std::endl;
    	exit(0);
    }

    if (result.count("positional") != 2)
    {
    	std::cout << "ERROR: check positional args" << std::endl << std::endl;
    	std::cout << options.help({"", "Group"}) << std::endl;
    	exit(0);
    }

    auto& positional = result["positional"].as<std::vector<std::string>>();

    params user_params(
            result["verbose"].as<bool>(),
            positional[0],
            positional[1],
            result.count("debug-dir") ? result["debug-dir"].as<std::string>() : std::string(""),
            result["size"].as<int>(),
            result["threshold"].as<int>(),
            !result["filter"].as<bool>(),
            result["landscape"].as<bool>(),
            result["partition"].as<float>()
    );

    user_params.show();

    if (user_params.isSavingDebugOutput()) {
    	mkdir(user_params.getDebugDir().c_str(), 0775);
    }

    std::cout << "Starting..." << std::endl;
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
    long inbetween_millis = millis;
     //Toy Story Color & breaks with median filter, needs filter() 48 pc
//    puzzle puzzle(input+"Toy Story/", 200, 22, false);

    
    //Toy Story back works w/ median filter 48pc
//    puzzle puzzle(input+"Toy Story back/", 200, 50);
    
    //Angry Birds color works with median, or filter 24 pc
//    puzzle puzzle(input+"Angry Birds/color/",300,30);

    //Angry Birds back works with median 24 pc
//    puzzle puzzle(input+"Angry Birds/Scanner Open/",300,30);
    
      //Horses back not numbered 104 pc
//    puzzle puzzle(input+"horses/", 380, 50);

    //Horses back numbered 104 pc
//    puzzle puzzle(input+"horses numbered/", 380, 50);

    puzzle puzzle(user_params);

    gettimeofday(&time, NULL);
    std::cout << std::endl << "time to initialize:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    
    puzzle.solve();
    gettimeofday(&time, NULL);
    std::cout << std::endl << "time to solve:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    inbetween_millis = ((time.tv_sec * 1000) + (time.tv_usec / 1000));
    puzzle.save_image();
    gettimeofday(&time, NULL);
    std::cout << std::endl << "Time to draw:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-inbetween_millis)/1000.0 << std::endl;
    
    
    gettimeofday(&time, NULL);
    std::cout << std::endl << "total time:"  << (((time.tv_sec * 1000) + (time.tv_usec / 1000))-millis)/1000.0 << std::endl;

    // system("/usr/bin/eog " + output_filename);
    
    return 0;
}


