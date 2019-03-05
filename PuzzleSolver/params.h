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
    bool solving;
    bool verbose;
    std::string inputDir;
    std::string outputDir;
    std::string solutionFileBasename;
    std::string pieceOrder;
    int estimatedPieceSize;
    int threshold;
    bool useMedianFilter;
    float partitionFactor;
    int minCornersQuality;
    bool saveAll;
    bool savingOriginals;
    bool savingContours;
    bool savingBlackWhite;
    bool savingColor;
    bool savingCorners;
    bool savingEdges;
    int findCornersBlockSize;
    bool editingCorners;
    float cornerEditorScale;

public:
    params();

    bool isSolving() const;

    void setSolving(bool solving);
    
    bool isVerbose() const;

    void setVerbose(bool Verbose);    

    std::string getInputDir() const;

    void setInputDir(std::string inputDir);
    
    std::string getOutputDir() const;

    void setOutputDir(std::string outputDir);
    
    std::string getSolutionFileBasename() const;

    void setSolutionFileBasename(std::string solutionImageFilename);
    
    std::string getPieceOrder() const;

    void setPieceOrder(std::string pieceOrder);
    
    int getEstimatedPieceSize() const;

    void setEstimatedPieceSize(int estimatedPieceSize);
    
    float getPartitionFactor() const;

    void setPartitionFactor(float partitionFactor);

    int getThreshold() const;

    void setThreshold(int threshold);

    bool isUsingMedianFilter() const;

    void setUsingMedianFilter(bool useMedianFilter);
    
    int getMinCornersQuality() const;

    void setMinCornersQuality(int minCornersQuality);

    void setSaveAll(bool writeAll);
    
    bool isSavingOriginals() const;

    void setSavingOriginals(bool savingOriginals);

    bool isSavingBlackWhite() const;

    void setSavingBlackWhite(bool savingBlackWhite);

    bool isSavingColor() const;

    void setSavingColor(bool savingColor);

    bool isSavingContours() const;

    void setSavingContours(bool savingContours);

    bool isSavingCorners() const;

    void setSavingCorners(bool savingCorners);

    bool isSavingEdges() const;

    void setSavingEdges(bool savingEdges);    
    
    int getFindCornersBlockSize() const;

    void setFindCornersBlockSize(int findCornersBlockSize);
    
    float getCornerEditorScale() const;

    void setCornerEditorScale(float cornerEditorScale);

    bool isEditingCorners() const;

    void setEditingCorners(bool editingCorners);
    
    void show() const;
    virtual ~params();



};

#endif /* PARAMS_H_ */
