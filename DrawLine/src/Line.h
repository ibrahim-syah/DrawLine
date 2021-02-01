#pragma once
#include <vector>

class Line
{
private:
    int m_pStart[2], m_pFinal[2]; // points coordinate
    int m_width, m_height;
public:
    Line(int _pStart[2], int _pFinal[2], int width, int height);
    ~Line();

    void convertToNDC(int _x, int _y, float *r_x, float *r_y);

    std::vector<float> createPoints(const unsigned int _pattern);
};