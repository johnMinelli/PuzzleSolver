/*
 * params.cpp
 *
 */

#include "params.h"

#include <iostream>

params::params() {
}

bool params::isSolving() const {
    return solving;
}

void params::setSolving(bool solving) {
    this->solving = solving;
}
    
uint params::getEstimatedPieceSize() const {
    return estimatedPieceSize;
}

void params::setEstimatedPieceSize(uint estimatedPieceSize) {
    this->estimatedPieceSize = estimatedPieceSize;
}

std::string params::getPieceOrder() const {
    return pieceOrder;
}

void params::setPieceOrder(std::string pieceOrder) {
    this->pieceOrder = pieceOrder;
}

uint params::getInitialPieceId() const {
    return initialPieceId;
}

void params::setInitialPieceId(uint initialPieceId) {
    this->initialPieceId = initialPieceId;
}
    
void params::setSaveAll(bool saveAll)
{
    this->saveAll = saveAll;
}

bool params::isSavingOriginals() const {
    return savingOriginals || saveAll;
}

void params::setSavingOriginals(bool savingOriginals) {
    this->savingOriginals = savingOriginals;
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

bool params::isSavingMatches() const {
    return savingMatches || saveAll;
}

void params::setSavingMatches(bool savingMatches) {
    this->savingMatches = savingMatches;
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

std::string params::getSolutionFileBasename() const {
    return solutionFileBasename;
}

void params::setSolutionFileBasename(std::string solutionImageFilename) {
    this->solutionFileBasename = solutionImageFilename;
}

uint params::getThreshold() const {
    return threshold;
}

void params::setThreshold(uint threshold) {
    this->threshold = threshold;
}

bool params::isUsingMedianFilter() const {
    return useMedianFilter;
}

void params::setUsingMedianFilter(bool useMedianFilter) {
    this->useMedianFilter = useMedianFilter;
}

uint params::getMedianBlurKSize() const {
    return medianBlurKSize;
}

void params::setMedianBlurKSize(uint medianBlurKSize) {
    this->medianBlurKSize = medianBlurKSize;
}
    
uint params::getFindCornersBlockSize() const {
    return findCornersBlockSize;
}

void params::setFindCornersBlockSize(uint findCornersBlockSize) {
    this->findCornersBlockSize = findCornersBlockSize;
}
    
uint params::getMinCornersQuality() const {
    return minCornersQuality;
}

void params::setMinCornersQuality(uint minCornersQuality) {
    this->minCornersQuality = minCornersQuality;
}

float params::getCornerEditorScale() const {
    return cornerEditorScale;
}

void params::setCornerEditorScale(float cornerEditorScale) {
    this->cornerEditorScale = cornerEditorScale;
}

bool params::isEditingCorners() const {
    return editingCorners;
}

void params::setEditingCorners(bool editingCorners) {
    this->editingCorners = editingCorners;
}

void params::show() const {
    std::cout << "verbose:                 " << (isVerbose() ? "true" : "false") << std::endl;
    std::cout << "input images dir:        " << this->getInputDir() << std::endl;
    std::cout << "output dir:              " << this->getOutputDir() << std::endl;
    std::cout << "solution name:           " << this->getSolutionFileBasename() << std::endl;
    std::cout << "solve puzzle:            " << (this->isSolving() ? "true" : "false") << std::endl;    
    std::cout << "estimated piece size:    " << this->getEstimatedPieceSize() << std::endl;
    std::cout << "threshold:               " << this->getThreshold() << std::endl;
    std::cout << "median filter:           " << (this->isUsingMedianFilter() ? "true" : "false") << std::endl;
    std::cout << "median blur ksize:       " << (this->getMedianBlurKSize()) << std::endl;
    std::cout << "piece order:             " << this->getPieceOrder() << std::endl;        
    std::cout << "partition factor:        " << this->getPartitionFactor() << std::endl;   
    std::cout << "find corners block size: " << this->getFindCornersBlockSize() << std::endl;
    std::cout << "min corners quality:     " << this->getMinCornersQuality() << std::endl;
    std::cout << "edit corners:            " << (this->isEditingCorners() ? "true" : "false") << std::endl;          
    std::cout << "corner editor scale:     " << this->getCornerEditorScale() << std::endl;     
    std::cout << "save original images:    " << (this->isSavingOriginals() ? "true" : "false") << std::endl;
    std::cout << "save contour images:     " << (this->isSavingContours() ? "true" : "false") << std::endl;
    std::cout << "save b&w images:         " << (this->isSavingBlackWhite() ? "true" : "false") << std::endl;    
    std::cout << "save color images:       " << (this->isSavingColor() ? "true" : "false") << std::endl;        
    std::cout << "save corner images:      " << (this->isSavingCorners() ? "true" : "false") << std::endl;        
    std::cout << "save edge images:        " << (this->isSavingEdges() ? "true" : "false") << std::endl;      
    std::cout << "save matched edges:      " << (this->isSavingMatches() ? "true" : "false") << std::endl;          
}

params::~params() {
    // TODO Auto-generated destructor stub
}


