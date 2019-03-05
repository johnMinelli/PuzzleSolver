/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   edit_corners.h
 * Author: kellinwood
 *
 * Created on March 4, 2019, 6:54 PM
 */

#ifndef EDIT_CORNERS_H
#define EDIT_CORNERS_H

/** @brief Display a GUI which allows the corners of a piece to be manually edited.

Returns true if the corners are moved to new locations, false otherwise.
Keyboard presses on 'q', 'n' or 'Enter' will end the editing operation, causing this method to return.
A keyboard press on 'r' will reset the editor to its original, unedited state.

@param window_name The name to use for the highgui window.
@param image The image of the puzzle piece
@param scale_factor The image shown in the editor is scaled up by this factor to make editing easier
@param original_corners The unedited (input) corners
@param edited_corners If the corners are moved, this vector is populated with the new corner locations.
@param verbose If true, some information will be sent to the console during the edit process.
*/
bool edit_corners(std::string& window_name, cv::Mat& image, float scale_factor, std::vector<cv::Point2f>& original_corners, std::vector<cv::Point2f>& edited_corners, bool verbose);

#endif /* EDIT_CORNERS_H */

