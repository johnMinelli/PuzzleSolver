/*
 * params.cpp
 *
 *  Created on: Jan 5, 2019
 *      Author: kellinwood
 */

#include "params.h"

#include <iostream>

params::params(bool is_verbose, std::string input_dir, std::string output_filename,
		std::string debug_dir, int estimated_piece_size, int threshold,
		bool use_median_filter, bool use_landscape, float partition_factor) {

    this->is_verbose = is_verbose;
    if (input_dir.back() != '/') input_dir = input_dir + "/";
    if (!debug_dir.empty() && debug_dir.back() != '/') debug_dir = debug_dir + "/";
    
    this->input_dir = input_dir;
    this->output_filename = output_filename;
    this->debug_dir = debug_dir;
    this->estimated_piece_size = estimated_piece_size;
    this->threshold = threshold;
    this->use_median_filter = use_median_filter;
    this->use_landscape = use_landscape;
    this->partition_factor = partition_factor;
}

bool params::isVerbose() const {
    return is_verbose;
}

std::string params::getInputDir() const {
    return input_dir;
}

std::string params::getOutputFile() const {
    return output_filename;
}

bool params::isSavingDebugOutput() const {
    return !debug_dir.empty();
}

std::string params::getDebugDir() const {
    return debug_dir;
}

int params::getEstimatedPieceSize() const {
    return estimated_piece_size;
}

int params::getThreshold() const {
    return threshold;
}

bool params::isUsingMedianFilter() const {
    return use_median_filter;
}

bool params::isUsingLandscape() const {
    return use_landscape;
}

float params::getPartitionFactor() const {
    return partition_factor;
}

void params::show() const {
    std::cout << "verbose:              " << (is_verbose ? "true" : "false") << std::endl;
    std::cout << "input_images_dir:     " << input_dir << std::endl;
    std::cout << "output_filename:      " << output_filename << std::endl;
    std::cout << "debug_dir:            " << (debug_dir.empty() ? "(not set)" : debug_dir) << std::endl;
    std::cout << "estimated_piece_size: " << estimated_piece_size << std::endl;
    std::cout << "threshold:            " << threshold << std::endl;
    std::cout << "median_filter:        " << (use_median_filter ? "true" : "false") << std::endl;
    std::cout << "landscape:            " << (use_landscape ? "true" : "false") << std::endl;        
    std::cout << "partition factor:     " << partition_factor << std::endl;                
}

params::~params() {
    // TODO Auto-generated destructor stub
}


