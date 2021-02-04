#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

#include <json/json.h>

class Line
{
private:
    int m_pStart[2], m_pFinal[2]; // points coordinate
    int m_width, m_height;

    Json::Value m_root;
public:
    Line();
    Line(int _pStart[2], int _pFinal[2], int width, int height);
    ~Line();

    void convertToNDC(int _x, int _y, float *r_x, float *r_y);

    std::vector<float> createPoints(const unsigned int _pattern, const int _lineWidth);

    void writeJSON(const char* filename, const float point_size, const int pattern, const float clear_color[4], const float line_color[4], const int lineWidth);
    //void readJSON(const char* filename);
    void readJSON(const char* filename, int pStart[2], int pFinal[2], float *point_size, int *pattern, float clear_color[4], float line_color[4], int *lineWidth);
};