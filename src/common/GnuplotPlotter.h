#ifndef GNUPLOTPLOTTER_H
#define GNUPLOTPLOTTER_H

#include <vector>
#include <string>
using namespace std;

#include "util.h"
using namespace util;

string format(const char *fmt, ...);

class GnuplotPlotter
{
public:
    GnuplotPlotter();

    /////////////////////////////////////////////
    // get
    /////////////////////////////////////////////
    string title() {return _title;}

    /////////////////////////////////////////////
    // set
    /////////////////////////////////////////////
    void setTitle(string title);
    void setAutoFit(bool autoFit) {_autoFit = autoFit;}

    /////////////////////////////////////////////
    // output file
    /////////////////////////////////////////////
    void outputPlotFile(string filePathName);
    void outputPdfFile(string filePathName);
    void outputPngFile(string filePathName);

    void outputPlotFileFence(string filePathName);

    /////////////////////////////////////////////
    // add/clear objects
    /////////////////////////////////////////////

    //index == -1 if no fence region
    void setNumOfFence(const unsigned& numFence) { numOfFence = numFence; }
    void addRectangleRegion(const Rect &rectangle, const int &regionIndex); 
    void addRectangleFill(const Rect &rectangle, const int &regionIndex);
    void addRectangle(const Rect &rectangle);
    void clearObjects();

private:
    // rectangle
    vector<Rect> _rectangles;
    // rect and fence region
    vector<pair<Rect,int> > _rectNRegions;
    vector<pair<Rect,int> > _filled_rects;
    unsigned numOfFence;

    vector<string> _colorStrs;      //newly add for fence region color

    // properties
    string _title;
    bool _autoFit;

};

#endif // GNUPLOTPLOTTER_H
