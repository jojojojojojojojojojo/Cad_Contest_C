#include "GnuplotPlotter.h"

#include <fstream>
#include <limits>
#include <cstdarg>
#include <sys/resource.h>

string format(const char *fmt, ...)
{
    va_list arg;

    // get string length & allocate memory
    va_start(arg,fmt);
    size_t len = vsnprintf(NULL, 0, fmt, arg);
    string buf(len, '\0');
    va_end(arg);

    // write string to buf
    va_start(arg,fmt);
    vsnprintf((char *)buf.c_str(), len+1, fmt, arg);
    va_end(arg);

    return buf;
}

GnuplotPlotter::GnuplotPlotter()
{
    _autoFit = true;
    // type "show palette colornames" in gnuplot console to show all color names
    //_colorStrs.push_back("blue");    //default blue (color for cells having no fence region)
    _colorStrs.push_back("red");   
    _colorStrs.push_back("purple");
    _colorStrs.push_back("brown");
    _colorStrs.push_back("gray");
    _colorStrs.push_back("dark-yellow");
    _colorStrs.push_back("dark-green");
    _colorStrs.push_back("orange-red");
    _colorStrs.push_back("dark-violet");
    _colorStrs.push_back("dark-goldenrod");
    _colorStrs.push_back("midnight-blue");
    _colorStrs.push_back("web-green");
    _colorStrs.push_back("dark-spring-green");
    _colorStrs.push_back("royalblue");
    _colorStrs.push_back("steelblue");
    _colorStrs.push_back("dark-pink");
    _colorStrs.push_back("dark-plum");
    _colorStrs.push_back("mediumpurple3");
    _colorStrs.push_back("navy");
}

void GnuplotPlotter::outputPngFile(string filePathName)
{
    ofstream file(filePathName.c_str());

    // put both commands and data in a single file (see gnuplot FAQ)
    file << format("set output \"%s.png\" ", filePathName.c_str()) << endl;
    file << format("set term png") << endl;

    file << format("set title \"%s\" ", _title.c_str()) << endl;
    file << format("set size ratio -1") << endl;
    file << format("set nokey") << endl;
    file << "set tics scale 0" << endl;

    // auto fit
    if (_autoFit == true) {
        double left, bottom, right, top;
        left = bottom = numeric_limits<double>::max();
        right = top = - numeric_limits<double>::max();
        for (unsigned i = 0; i < _rectangles.size(); i++) {
            Rect &rectangle = _rectangles[i];
            left = min(left, rectangle.left());
            bottom = min(bottom, rectangle.bottom());
            right = max(right, rectangle.right());
            top = max(top, rectangle.top());
        }
        file << format("set xrange[%f:%f]", left, right) << endl;
        file << format("set yrange[%f:%f]", bottom, top) << endl;
    }

    // plot rectangle
    file << "plot '-' with lines linetype 3" << endl;
    for (unsigned i = 0; i < _rectangles.size(); i++) {
        Rect &rectangle = _rectangles[i];
        /*
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << endl;
        */

        file << format("%d %d", (int)rectangle.left(), (int)rectangle.bottom()) << endl;
        file << format("%d %d", (int)rectangle.right(), (int)rectangle.bottom()) << endl;
        file << format("%d %d", (int)rectangle.right(), (int)rectangle.top()) << endl;
        file << format("%d %d", (int)rectangle.left(), (int)rectangle.top()) << endl;
        file << format("%d %d", (int)rectangle.left(), (int)rectangle.bottom()) << endl;
        file << endl;
    }
    file << "EOF" << endl;

    system(format("gnuplot %s", filePathName.c_str()).c_str());
}


void GnuplotPlotter::outputPdfFile(string filePathName)
{
    ofstream file(filePathName.c_str());

    // put both commands and data in a single file (see gnuplot FAQ)
    file << format("set output \"%s.pdf\" ", filePathName.c_str()) << endl;
    file << format("set term pdf") << endl;

    file << format("set title \"%s\" ", _title.c_str()) << endl;
    file << format("set size ratio -1") << endl;
    file << format("set nokey") << endl;
    file << "set tics scale 0" << endl;

    // auto fit
    if (_autoFit == true) {
        double left, bottom, right, top;
        left = bottom = numeric_limits<double>::max();
        right = top = - numeric_limits<double>::max();
        for (unsigned i = 0; i < _rectangles.size(); i++) {
            Rect &rectangle = _rectangles[i];
            left = min(left, rectangle.left());
            bottom = min(bottom, rectangle.bottom());
            right = max(right, rectangle.right());
            top = max(top, rectangle.top());
        }
        file << format("set xrange[%f:%f]", left, right) << endl;
        file << format("set yrange[%f:%f]", bottom, top) << endl;
    }

    // plot rectangle
    file << "plot '-' with lines linetype 2" << endl;
    for (unsigned i = 0; i < _rectangles.size(); i++) {
        Rect &rectangle = _rectangles[i];
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << endl;
    }
    file << "EOF" << endl;

    system(format("gnuplot %s", filePathName.c_str()).c_str());
}

void GnuplotPlotter::outputPlotFile(string filePathName)
{
    ofstream file(filePathName.c_str());
    file<<"reset"<<endl;

    // put both commands and data in a single file (see gnuplot FAQ)
    file << format("set title \"%s\" ", _title.c_str()) << endl;
    file << format("set size ratio -1") << endl;
    file << format("set nokey") << endl;
    file << "set tics scale 0" << endl;

    // auto fit
    if (_autoFit == true) {
        double left, bottom, right, top;
        left = bottom = numeric_limits<double>::max();
        right = top = - numeric_limits<double>::max();
        for (unsigned i = 0; i < _rectangles.size(); i++) {
            Rect &rectangle = _rectangles[i];
            left = min(left, rectangle.left());
            bottom = min(bottom, rectangle.bottom());
            right = max(right, rectangle.right());
            top = max(top, rectangle.top());
        }
        file << format("set xrange[%f:%f]", left, right) << endl;
        file << format("set yrange[%f:%f]", bottom, top) << endl;
    }

    // plot rectangle
    //set object 1 rect from 0,0 to 20000,20000 lw 1 fs empty border lc rgb 1
    /*
    file << "plot 0" << endl;
    file << format("set style rect back lw 1 fs empty border lc rgb \"%s\"","blue")<<endl;
    for (unsigned i = 0; i < _rectangles.size(); i++) {
        Rect &rectangle = _rectangles[i];
        file << format("set object rect from %f,%f to %f,%f ", rectangle.left(), rectangle.bottom(), rectangle.right(), rectangle.top()) << endl;
        //file << endl;
    }*/
    file << "plot '-' with lines linetype 3" << endl;
    for (unsigned i = 0; i < _rectangles.size(); i++) {
        Rect &rectangle = _rectangles[i];
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << endl;
    }
    file << "EOF" << endl;
    


    // wait
    file << "pause -1 'Press any key'" << endl;
}

void GnuplotPlotter::outputPlotFileFence(string filePathName)
{
    ofstream file(filePathName.c_str());
    file<<"reset"<<endl;

    // put both commands and data in a single file (see gnuplot FAQ)
    file << format("set title \"%s\" ", _title.c_str()) << endl;
    file << format("set size ratio -1") << endl;
    file << format("set nokey") << endl;
    file << "set tics scale 0" << endl;

    // auto fit
    if (_autoFit == true) {
        double left, bottom, right, top;
        left = bottom = numeric_limits<double>::max();
        right = top = - numeric_limits<double>::max();
        for (unsigned i = 0; i < _rectNRegions.size(); i++) {
            Rect &rectangle = _rectNRegions[i].first;
            left = min(left, rectangle.left());
            bottom = min(bottom, rectangle.bottom());
            right = max(right, rectangle.right());
            top = max(top, rectangle.top());
        }
        file << format("set xrange[%f:%f]", left, right) << endl;
        file << format("set yrange[%f:%f]", bottom, top) << endl;
    }

    file << format("set style rect back lw 1 ")<<endl;
    for(unsigned k = 0 ; k < numOfFence ; k++)
    {
        for (unsigned i = 0; i < _rectNRegions.size(); i++) {
            if(_rectNRegions[i].second != (int)k) { continue; }
            Rect &rectangle = _rectNRegions[i].first;
            file << format("set object rect from %f,%f to %f,%f fs empty border lc rgb \"%s\"", rectangle.left(), rectangle.bottom(), rectangle.right(), rectangle.top(), _colorStrs[k].c_str()) << endl;
        }
    }
    file << "plot '-' with lines linetype 3" << endl;
    for (unsigned i = 0; i < _rectNRegions.size(); i++) {
        if(_rectNRegions[i].second != -1) { continue; }
        Rect &rectangle = _rectNRegions[i].first;
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.bottom()) << endl;
        file << format("%f %f", rectangle.right(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.top()) << endl;
        file << format("%f %f", rectangle.left(), rectangle.bottom()) << endl;
        file << endl;
    }
    file << "EOF" << endl;
    


    // wait
    file << "pause -1 'Press any key'" << endl;
}

void GnuplotPlotter::addRectangleRegion(const Rect &rectangle, const int &regionIndex)
{
    _rectNRegions.push_back(make_pair(rectangle,regionIndex));
    assert(regionIndex < (int)numOfFence);
}

void GnuplotPlotter::addRectangle(const Rect &rectangle)
{
    _rectangles.push_back(rectangle);
}

void GnuplotPlotter::clearObjects()
{
    _rectangles.clear();
}

void GnuplotPlotter::setTitle(string title)
{
    _title = title;
}
