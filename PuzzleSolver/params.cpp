/*
 * params.cpp
 *
 *  Created on: Jan 5, 2019
 *      Author: kellinwood
 */

#include "params.h"

#include <iostream>

params::params() {
}

int params::getEstimatedPieceSize() const {
    return estimatedPieceSize;
}

void params::setEstimatedPieceSize(int estimatedPieceSize) {
    this->estimatedPieceSize = estimatedPieceSize;
}

void params::setSaveAll(bool saveAll)
{
    this->saveAll = saveAll;
}

bool params::isSavingBlackWhite() const {
    return savingBlackWhite || saveAll;
}

void params::setSavingBlackWhite(bool savingBlackWhite) {
    this->savingBlackWhite = savingBlackWhite;
}

bool params::isSavingColor() const {
    return savingColor || saveAll;
}

void params::setSavingColor(bool savingColor) {
    this->savingColor = savingColor;
}

bool params::isSavingContours() const {
    return savingContours || saveAll;
}

void params::setSavingContours(bool savingContours) {
    this->savingContours = savingContours;
}

bool params::isSavingCorners() const {
    return savingCorners || saveAll;
}

void params::setSavingCorners(bool savingCorners) {
    this->savingCorners = savingCorners;
}

bool params::isSavingEdges() const {
    return savingEdges || saveAll;
}

void params::setSavingEdges(bool savingEdges) {
    this->savingEdges = savingEdges;
}

std::string params::getInputDir() const {
    return inputDir;
}

void params::setInputDir(std::string inputDir) {
    if (inputDir.back() != '/') inputDir = inputDir + "/";
    this->inputDir = inputDir;
}

bool params::isVerbose() const {
    return verbose;
}

void params::setVerbose(bool Verbose) {
    verbose = Verbose;
}

std::string params::getOutputDir() const {
    return outputDir;
}

void params::setOutputDir(std::string outputDir) {
    if (outputDir.back() != '/') outputDir = outputDir + "/";
    this->outputDir = outputDir;
}

float params::getPartitionFactor() const {
    return partitionFactor;
}

void params::setPartitionFactor(float partitionFactor) {
    this->partitionFactor = partitionFactor;
}

std::string params::getSolutionImageFilename() const {
    return solutionImageFilename;
}

void params::setSolutionImageFilename(std::string solutionImageFilename) {
    this->solutionImageFilename = solutionImageFilename;
}

int params::getThreshold() const {
    return threshold;
}

void params::setThreshold(int threshold) {
    this->threshold = threshold;
}

bool params::isUsingLandscape() const {
    return useLandscape;
}

void params::setUsingLandscape(bool useLandscape) {
    this->useLandscape = useLandscape;
}

bool params::isUsingMedianFilter() const {
    return useMedianFilter;
}

void params::setUsingMedianFilter(bool useMedianFilter) {
    this->useMedianFilter = useMedianFilter;
}

int params::getMinCornersQuality() const {
    return minCornersQuality;
}

void params::setMinCornersQuality(int minCornersQuality) {
    this->minCornersQuality = minCornersQuality;
}


void params::show() const {
    std::cout << "verbose:              " << (isVerbose() ? "true" : "false") << std::endl;
    std::cout << "input images dir:     " << getInputDir() << std::endl;
    std::cout << "output dir:           " << getOutputDir() << std::endl;
    std::cout << "save contour images:  " << (this->isSavingContours() ? "true" : "false") << std::endl;
    std::cout << "save b&w images:      " << (this->isSavingBlackWhite() ? "true" : "false") << std::endl;    
    std::cout << "save color images:    " << (this->isSavingColor() ? "true" : "false") << std::endl;        
    std::cout << "save corner images:   " << (this->isSavingCorners() ? "true" : "false") << std::endl;        
    std::cout << "save edge images:     " << (this->isSavingEdges() ? "true" : "false") << std::endl;            
    std::cout << "estimated piece size: " << getEstimatedPieceSize() << std::endl;
    std::cout << "threshold:            " << getThreshold() << std::endl;
    std::cout << "median filter:        " << (isUsingMedianFilter() ? "true" : "false") << std::endl;
    std::cout << "landscape:            " << (isUsingLandscape() ? "true" : "false") << std::endl;        
    std::cout << "partition factor:     " << getPartitionFactor() << std::endl;                
    std::cout << "min corners quality:  " << getMinCornersQuality() << std::endl;
}

params::~params() {
    // TODO Auto-generated destructor stub
}

