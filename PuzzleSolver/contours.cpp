/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "contours.h"

#include <bits/stdc++.h>
#include <opencv2/core.hpp>

contour::contour(cv::Rect _bounds, std::vector<cv::Point> _points) : bounds(_bounds), points(_points) {}

contour_partition::contour_partition(int index) {
    this->index = index;
    offset = INT_MAX;
}

void contour_partition::update_offset(int off) {
    offset = std::min(offset, off);
}

    


contour_mgr::contour_mgr(int _container_width, int _container_height, params& _user_params) : user_params(_user_params) {
    container_width = _container_width;
    container_height = _container_height;
}

void contour_mgr::add_contour(cv::Rect _bounds, std::vector<cv::Point> _points) {
    contours.push_back(contour(_bounds,_points));
}

// Sort the contours so that the pieces end up being identified based on their position in the original image --
// i.e., assuming the pieces are arranged in a grid in the image, then number them left to right going
// from the top to the bottom.  The point of doing this is to provide some way to correlate hand-written piece
// numbers with the numerical (text) output of this program and is especially helpful if a solution is found
// but a final solution image can't be generated. The piece layout in the image does not need to be exact, 
// but the differences in y of each row (or x of each column) must be less than the estimated piece size multiplied 
// by the partition factor.  If "landscape" is true, then pieces are ordered top to bottom going left to right.
void contour_mgr::sort_contours() {
    // First, partition the contours based on x (or y) position.
    std::vector<int> labels;

    // partition the contours into rows (or columns if landscape==true)
    cv::partition(contours, labels, [=](const contour& a, const contour& b) {
        int diff = user_params.isUsingLandscape() ? (a.bounds.x - b.bounds.x) : (a.bounds.y - b.bounds.y);
        return std::abs(diff) < user_params.getEstimatedPieceSize() * user_params.getPartitionFactor();
    });

    // Determine the number of partitions
    int num_partitions = 0;
    for (uint i = 0; i < labels.size(); i++) {
        num_partitions = std::max(labels[i], num_partitions);
    }
    num_partitions += 1;

    // Create an array of partition objects
    contour_partition* partition_array[num_partitions];
    // Create a sortable vector of partitions
    std::vector<contour_partition*> partition_vector;
    for (uint i = 0; i < num_partitions; i++) {
        partition_array[i] = new contour_partition(i);
        partition_vector.push_back(partition_array[i]);
    }

    // Determine the min offset (x or y) for each partition
    for (uint i = 0; i < contours.size(); i++) {
        int partition = labels[i];
        int current_offset = partition_array[partition]->offset;
        int extent = user_params.isUsingLandscape() ? contours[i].bounds.x : contours[i].bounds.y;
        partition_array[partition]->offset = std::min(extent, current_offset);
    }

    // Sort the partitions into offset order
    std::sort(partition_vector.begin(), partition_vector.end(), [](contour_partition* a, contour_partition* b) {
        return (a->offset) < (b-> offset);
    });

    // Assign the partition order attribute based on the sorted order
    for (uint i = 0 ; i < partition_vector.size(); i++) {
        partition_vector[i]->order = i;
    }

    int container_dimension = user_params.isUsingLandscape() ? container_height : container_width;
    // Assign the sort_factor to each contour
    for (uint i = 0; i < contours.size(); i++) {

        int contour_bounds_position = user_params.isUsingLandscape() ? contours[i].bounds.y : contours[i].bounds.x;
        contours[i].sort_factor = partition_array[labels[i]]->order * container_dimension + contour_bounds_position;
    }

    // Sort the contours
    std::sort(contours.begin(), contours.end(),
        [](const contour & a, const contour & b) -> bool
    {
        return a.sort_factor < b.sort_factor;
    });        
}