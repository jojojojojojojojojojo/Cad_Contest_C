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
    void setWithEdge(bool _edge) {_withEdge = _edge;}

    /////////////////////////////////////////////
    // output file
    /////////////////////////////////////////////
    void outputPlotFile(string filePathName);
    void outputPdfFile(string filePathName);
    void outputPngFile(string filePathName);

    void outputPlotFileFence(string filePathName);

    void addArrowInPlot(ofstream& of);

    /////////////////////////////////////////////
    // add/clear objects
    /////////////////////////////////////////////

    //index == -1 if no fence region
    void setNumOfFence(const unsigned& numFence) { numOfFence = numFence; }
    void addRectangleRegion(const Rect &rectangle, const int &regionIndex); 
    void addRectangleFill(const Rect &rectangle, const int &regionIndex);
    void addRectangle(const Rect &rectangle);
    void addArrows(const double& from_x, const double& from_y, const double& to_x, const double& to_y)
    {
        _arrows.push_back(make_pair(make_pair(from_x,from_y), make_pair(to_x,to_y)));
    }

    void clearObjects();

private:
    // rectangle
    vector<Rect> _rectangles;
    // rect and fence region
    vector<pair<Rect,int> > _rectNRegions;
    vector<pair<Rect,int> > _filled_rects;
    unsigned numOfFence;

    // arrows
    vector<pair<pair<double,double>, pair<double,double> > > _arrows;

    vector<string> _colorStrs;      //newly add for fence region color

    // properties
    string _title;
    bool _autoFit;
    bool _withEdge;

};

#endif // GNUPLOTPLOTTER_H
