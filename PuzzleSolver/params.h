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
    bool verbose;
    std::string inputDir;
    std::string outputDir;
    std::string solutionImageFilename;
    bool generatingDebugOutput;
    int estimatedPieceSize;
    int threshold;
    bool useMedianFilter;
    bool useLandscape;
    float partitionFactor;
    int minCornersQuality;
    
    bool saveAll;
    bool savingContours;
    bool savingBlackWhite;
    bool savingColor;
    bool savingCorners;
    bool savingEdges;

public:
    params();

    int getEstimatedPieceSize() const ;

    void setEstimatedPieceSize(int estimatedPieceSize) ;

    void setSaveAll(bool writeAll);
    
    bool isSavingBlackWhite() const ;

    void setSavingBlackWhite(bool savingBlackWhite);

    bool isSavingColor() const ;

    void setSavingColor(bool savingColor);

    bool isSavingContours() const ;

    void setSavingContours(bool savingContours);

    bool isSavingCorners() const ;

    void setSavingCorners(bool savingCorners);

    bool isSavingEdges() const ;

    void setSavingEdges(bool savingEdges);
    
    std::string getInputDir() const ;

    void setInputDir(std::string inputDir) ;

    bool isVerbose() const ;

    void setVerbose(bool Verbose) ;

    std::string getOutputDir() const ;

    void setOutputDir(std::string outputDir) ;

    float getPartitionFactor() const ;

    void setPartitionFactor(float partitionFactor) ;

    std::string getSolutionImageFilename() const ;

    void setSolutionImageFilename(std::string solutionImageFilename) ;

    int getThreshold() const ;

    void setThreshold(int threshold) ;

    bool isUsingLandscape() const ;

    void setUsingLandscape(bool useLandscape) ;

    bool isUsingMedianFilter() const ;

    void setUsingMedianFilter(bool useMedianFilter) ;
    
    int getMinCornersQuality() const;

    void setMinCornersQuality(int minCornersQuality);

    void show() const;
    virtual ~params();



};

#endif /* PARAMS_H_ */
