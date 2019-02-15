/*
 * params.hpp
 *
 *  Created on: Jan 5, 2019
 *      Author: kellinwood
 */

#ifndef PARAMS_H_
#define PARAMS_H_

#include <string>

class params {
private:
    bool is_verbose;
    std::string input_dir;
    std::string output_filename;
    std::string debug_dir;
    int estimated_piece_size;
    int threshold;
    bool use_median_filter;
    bool use_landscape;
    float partition_factor;
public:
    params(bool is_verbose, std::string input_dir, std::string output_filename, std::string debug_dir, 
            int estimated_piece_size, int threshold, bool use_median_filter, bool landscape,
            float partition_factor);
    bool isVerbose() const;
    std::string getInputDir() const;
    std::string getOutputFile() const;
    bool isSavingDebugOutput() const;
    std::string getDebugDir() const;
    int getEstimatedPieceSize() const;
    int getThreshold() const;
    bool isUsingMedianFilter() const;
    bool isUsingLandscape() const;
    float getPartitionFactor() const;
    void show() const;
    virtual ~params();
};

#endif /* PARAMS_H_ */
