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

    /////////////////////////////////////////////
    // add/clear objects
    /////////////////////////////////////////////
    void addRectangle(const Rect &rectangle);
    void clearObjects();

private:
    // rectangle
    vector<Rect> _rectangles;

    // properties
    string _title;
    bool _autoFit;

};

#endif // GNUPLOTPLOTTER_H
