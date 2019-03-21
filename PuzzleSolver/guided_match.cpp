#include "opencv2/opencv.hpp"

#include <vector>
#include "guided_match.h"
#include "utils.h"

void gm_mouse_callback(int event, int x, int y, int flags, void* userdata);

std::vector<cv::Point2f> rotate(cv::Point2f center, std::vector<cv::Point> cont, double angle) {
    
    std::vector<cv::Point2f> ret_contour;
    angle = angle * M_PI / 180.0;
    
    for(std::vector<cv::Point>::iterator i = cont.begin(); i != cont.end(); i++ ) {
        
        cv::Point2f t(i->x - center.x, i->y - center.y);
        double cosa = std::cos(angle);
        double sina = std::sin(angle);
        double new_x = cosa * t.x - sina * t.y;
        double new_y = sina * t.x + cosa * t.y;
        ret_contour.push_back(cv::Point2f((float)(new_x + center.x), (float)(new_y + center.y)));
    }
    return ret_contour;
}
    
class guided_matcher {
public:
    piece& p1;
    piece& p2;
    int e1;
    int e2;
    bool debug;

    bool pieces;
    bool edges;    
    bool color;
    int margin;
    
    int p2xoff = 0;
    int p2yoff = 0;
    
    
    params& user_params;
    
    float scale_factor;
    cv::Mat rendered;
    std::string window_name;

    
    guided_matcher(piece& p1, piece& p2, int e1, int e2, params& user_params)
        : p1(p1), p2(p2), e1(e1), e2(e2), user_params(user_params)
    {
        debug = false;
        color = true;
        edges = false;
        pieces = true;
        margin = 25;
        
        scale_factor = user_params.getGuiScale();
        
        std::stringstream wns;
        wns << p1.get_id() << "-" << e1 << " __ " << p2.get_id() << "-" << e2;
        window_name = wns.str();
        

    }
    
    cv::Mat bw_to_color(cv::Mat bw) {
        std::vector<cv::Mat> channels(3);
        channels.at(0) = bw;
        channels.at(1) = bw;
        channels.at(2) = bw;
        
        cv::Mat color;
        cv::merge(channels,color);
        return color;
    }
    
    template<class T>
    cv::Point_<T> get_top(std::vector<cv::Point_<T>>& contour, cv::Point_<T>* bottom = NULL) {
        cv::Point f = contour.front();
        cv::Point b = contour.back();
        if (f.y < b.y) {
            if (bottom != NULL) {
                bottom->x = b.x;
                bottom->y = b.y;
            }
            return f;
        } 
        else {
            if (bottom != NULL) {
                bottom->x = f.x;
                bottom->y = f.y;
            }
            return b;
        }
    }
    
    template<class T>
    double compute_angle(std::vector<cv::Point_<T>> contour) {
        cv::Point_<T> b;
        cv::Point_<T> a = get_top(contour, &b);
        cv::Point_<T> c = cv::Point_<T>(b.x, a.y);
        return compute_rotation_angle(a, b, c);        
    }
    

    std::vector<cv::Point> render_piece(piece& p, int edge, int extra_rotation, cv::Mat& dst, int xoffset, int yoffset, int padded_dim, cv::Point* align_to) {  
        cv::Mat orig;
        cv::Mat padded;
        cv::Mat aligned;
        cv::Mat resized;
        
        int alignx = 0;
        int aligny = 0;
        
        orig = color ? p.full_color : bw_to_color(p.bw);

        
        // Add alpha channel and initialize it using the bw image
        cv::Mat channels[4];
        cv::split(orig, channels);
        p.bw.convertTo(channels[3], channels[0].type());
        cv::merge(channels, 4, orig);
        
        
        // Calculate the translation into the padded area
        cv::Point tx((padded_dim - orig.size().width) / 2, (padded_dim - orig.size().height) / 2);
        
        // Calculate the center of the original
        cv::Point center(orig.cols/2.0, orig.rows/2.0);


        // Compute the rotation required to align the pieces
        
        // Angle to straighten edge 0
        double angle = compute_angle(p.edges[0].get_contour());

        // Update the angle to get the selected edge facing the correct direction
        // extra_rotation = 0 is used for the piece on the left, extra_rotation=2 is used on the right
        angle -= 90.0 * ((edge + extra_rotation) % 4 - 2);
        
        // Rotate the selected edge contour
        std::vector<cv::Point2f> edge_contour = rotate(center, p.edges[edge].get_contour(), -angle);
        
        // Fine tune the angle 
        angle += compute_angle(edge_contour);
        
        
        // Rotate and translate the selected edge
        edge_contour = rotate(center, p.edges[edge].get_contour(), -angle);
        std::vector<cv::Point> edge_contour_tx;
        translate_contour(edge_contour, edge_contour_tx, tx.x, tx.y, 1.0);        


        // Determine the translation required for alignment
        cv::Point top = get_top(edge_contour_tx);
        if (align_to != NULL) {
            alignx = align_to->x - top.x;
            aligny = align_to->y - top.y;
        }
        
        alignx += xoffset;
        aligny += yoffset;
        
        if (pieces) {
            // Copy the original into the padded image
            padded = cv::Mat::zeros( padded_dim, padded_dim, CV_8UC4);        
            orig.copyTo(padded(cv::Rect(tx.x, tx.y, orig.size().width, orig.size().height)));            
            
            // Rotate the padded image
            cv::Mat r = cv::getRotationMatrix2D(cv::Point2f(padded.cols/2.0, padded.rows/2.0), angle, 1.0);
            cv::warpAffine(padded, aligned, r, padded.size());               
            
            if (aligny != 0) {
                cv::Mat out = cv::Mat::zeros(aligned.size(), aligned.type());
                
                if (aligny > 0) {
                    aligned(cv::Rect(0,0, aligned.cols,aligned.rows-aligny))
                            .copyTo(out(cv::Rect(0,aligny,aligned.cols,aligned.rows-aligny)));                 
                } else {
                    aligned(cv::Rect(0,-aligny, aligned.cols,aligned.rows+aligny))
                            .copyTo(out(cv::Rect(0,0,aligned.cols,aligned.rows+aligny)));               
                }
                aligned = out;
            }
            
            // Resize
            cv::resize(aligned, resized, cv::Size(), scale_factor, scale_factor, cv::INTER_CUBIC);
            
            // Split to gain access to the padded, rotated, and resized alpha channel
            cv::split(resized, channels);
            
            // Copy using the alpha channel as the mask
            cv::Rect crect(alignx*scale_factor, 0, std::min(rendered.size().width, resized.size().width), std::min(rendered.size().height, resized.size().height));
            resized.copyTo(rendered(cv::Rect(crect)), channels[3]);  
        }
        
        // Render edge contour in red
        if (edges) {
            std::vector<std::vector<cv::Point>> edge_contours;
            edge_contours.push_back(translate_contour(edge_contour_tx, alignx, aligny, scale_factor));
            cv::polylines(rendered, edge_contours, false, cv::Scalar(0, 0, 255), 2);        
        }
        
        return edge_contour_tx;
    }
    
    /**
     * Draws a rectangle with rounded corners, the parameters are the same as in the OpenCV function @see rectangle();
     * @param cornerRadius A positive int value defining the radius of the round corners.
     */
    void rounded_rectangle( cv::Mat& src, cv::Point topLeft, cv::Point bottomRight, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius)
    {
        /* corners:
         * p1 - p2
         * |     |
         * p4 - p3
         */
        cv::Point p1 = topLeft;
        cv::Point p2 = cv::Point (bottomRight.x, topLeft.y);
        cv::Point p3 = bottomRight;
        cv::Point p4 = cv::Point (topLeft.x, bottomRight.y);
        
        // draw straight lines
        cv::line(src, cv::Point (p1.x+cornerRadius,p1.y), cv::Point (p2.x-cornerRadius,p2.y), lineColor, thickness, lineType);
        cv::line(src, cv::Point (p2.x,p2.y+cornerRadius), cv::Point (p3.x,p3.y-cornerRadius), lineColor, thickness, lineType);
        cv::line(src, cv::Point (p4.x+cornerRadius,p4.y), cv::Point (p3.x-cornerRadius,p3.y), lineColor, thickness, lineType);
        cv::line(src, cv::Point (p1.x,p1.y+cornerRadius), cv::Point (p4.x,p4.y-cornerRadius), lineColor, thickness, lineType);
        
        // draw arcs
        cv::ellipse( src, p1+cv::Point(cornerRadius, cornerRadius), cv::Size( cornerRadius, cornerRadius ), 180.0, 0, 90, lineColor, thickness, lineType );
        cv::ellipse( src, p2+cv::Point(-cornerRadius, cornerRadius), cv::Size( cornerRadius, cornerRadius ), 270.0, 0, 90, lineColor, thickness, lineType );
        cv::ellipse( src, p3+cv::Point(-cornerRadius, -cornerRadius), cv::Size( cornerRadius, cornerRadius ), 0.0, 0, 90, lineColor, thickness, lineType );
        cv::ellipse( src, p4+cv::Point(cornerRadius, -cornerRadius), cv::Size( cornerRadius, cornerRadius ), 90.0, 0, 90, lineColor, thickness, lineType );
    }

    void render() {
     
        int maxdim = std::max(std::max(std::max(
                p1.full_color.size().width, 
                p1.full_color.size().height), 
                p2.full_color.size().width), 
                p2.full_color.size().height);        
        
        int width = maxdim * 2 + margin * 4;

        int height = maxdim + margin * 2;

        int padded_dim = maxdim + margin * 2;
        
        width *= scale_factor;
        width += (width % 2) + 2;
        height *= scale_factor;
        height += (height % 2) + 2;
        
        rendered = cv::Mat::zeros(height, width, CV_8UC4 );

        std::vector<cv::Point> txc = render_piece(p1, e1, 0, rendered, 0, 0, padded_dim, NULL);

        cv::Point top = get_top(txc);
        render_piece(p2, e2, 2, rendered, p2xoff, p2yoff, padded_dim, &top);

        
        rounded_rectangle(rendered, cv::Point(10,10), cv::Point(80,50), cv::Scalar(0,150,255), 1, cv::LINE_AA, 10);
        
        cv::imshow(window_name, rendered);
    }

    void adjust_scale( float adjustment) {
        float new_scale =  scale_factor + adjustment;
        if (new_scale < 0.25) {
            new_scale = 0.25;
        }
        if (new_scale == scale_factor) {
            return;
        }
        scale_factor = new_scale;
        render();
        std::cout << "Scale factor is now " << scale_factor << std::endl;
    }
    
    std::string edit() {

        
        cv::namedWindow(window_name);    
        cv::setMouseCallback(window_name, gm_mouse_callback, this);
        
        render();

        std::string result;
        
        bool done = false;
        do {
            int c = cv::waitKey(0);
            // std::cout << c << std::endl;
            switch (c) {
                case -1:  // User probably clicked on the "x" in the window title bar
                // case 13:  // Return key
                // case 141: // Enter key
                case 'n': // "next"
                case 'q': // "quit"
                    done = true;
                    result = "no";
                    break;
                case 'y':
                    done = true;
                    result = "yes";
                    break;
                case 'c':
                    color = !color;
                    render();
                    break;
                case 'e':
                case 'p':
                    pieces = !pieces;
                    edges = !pieces;
                    render();
                    break;                    
                case 'm':
                    margin = (margin + 25) % 50;
                    std::cout << "Margin is now " << margin << std::endl;
                    render();
                    break;
                case ',':
                    e1 = (e1+1)%4;
                    std::cout << "E1 is now " << e1 << std::endl;
                    render();
                    break;
                case '.':
                    e2 = (e2+1)%4;
                    std::cout << "E2 is now " << e2 << std::endl;
                    render();
                    break;       
                case 'x':
                    p2xoff -= 1;
                    std::cout << "p2xoff is now " << p2xoff << std::endl;
                    render();
                    break;
                case 'X':
                    p2xoff += 1;
                    std::cout << "p2xoff is now " << p2xoff << std::endl;
                    render();
                    break;       
                case 'z':
                    p2yoff -= 1;
                    std::cout << "p2yoff is now " << p2yoff << std::endl;
                    render();
                    break;
                case 'Z':
                    p2yoff += 1;
                    std::cout << "p2yoff is now " << p2yoff << std::endl;
                    render();
                    break;                      
                case '-':
                    adjust_scale(-0.25);
                    break;
                case '+':
                case '=':
                    adjust_scale(0.25);
                    break;                    
                default:
                    break;
            }
        }
        while (!done);
        try {
            cv::destroyWindow(window_name);
        }
        catch (cv::Exception x) {
            // Ignore
        }
        
        return result;
    }
    
    
    void mouse_down(int x, int y) {
    }
    
    void mouse_move(int x, int y) {
    }
    
    void mouse_up(int x, int y) {

    }    
    
private:

}; 

void gm_mouse_callback(int event, int x, int y, int flags, void* userdata) {
    guided_matcher* data = (guided_matcher*)userdata;
    
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
        if (data->debug) {
            std::cout << "Left mouse down - position (" << x << ", " << y << ")" << std::endl;
        }
        data->mouse_down(x, y);
    }
    else if  ( event == cv::EVENT_LBUTTONUP )
    {
        if (data->debug) {
            std::cout << "Left mouse up - position (" << x << ", " << y << ")" << std::endl;
        }
        data->mouse_up(x, y);
    }    
    else if ( event == cv::EVENT_MOUSEMOVE )
    {
        data->mouse_move(x, y);
        // std::cout << "Mouse move - position (" << x << ", " << y << ")" << std::endl;
    }    
}


std::string guided_match(piece& p1, piece& p2, int e1, int e2, params& user_params) {
    guided_matcher editor(p1, p2, e1, e2, user_params);
    return editor.edit();
}

