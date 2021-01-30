#include "Line.h"

Line::Line(int _pStart[2], int _pFinal[2], int _width, int _height)
{
    m_width = _width;
    m_height = _height;
    m_pStart[0] = _pStart[0];
    m_pStart[1] = _pStart[1];
    m_pFinal[0] = _pFinal[0];
    m_pFinal[1] = _pFinal[1];
}

Line::~Line()
{
}

void Line::convertToNDC(int _x, int _y, float *r_x, float *r_y)
{
    // convert into opengl vec2 range (-1 to 1)
    *r_x = 2.0f * _x / m_width - 1.0f;
    *r_y = 2.0f * _y / m_height - 1.0f;
}


std::vector<float> Line::createPoints()
{
    std::vector<float> points = {};
    if (m_pStart[1] == m_pFinal[1]) // horizontal line
    {
        int yvalue = m_pStart[1];
        int len = m_pStart[0] - m_pFinal[0];
        if (len < 0) // "rightward"
        {
            int x = m_pStart[0];
            for (int i = 0; x < m_pFinal[0]; i++)
            {
                float ndc_x, ndc_y;
                this->convertToNDC(x, yvalue, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, 0.0f };
                points.insert(points.end(), point.begin(), point.end());
                x++;
            }
        }
        else if (len > 0) // "leftward"
        {
            int x = m_pFinal[0];
            for (int i = 0; x < m_pStart[0]; i++)
            {
                float ndc_x, ndc_y;
                this->convertToNDC(x, yvalue, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, 0.0f };
                points.insert(points.end(), point.begin(), point.end());
                x++;
            }

        }
        else // m_pStart = m_pFinal
        {
            float ndc_x, ndc_y;
            this->convertToNDC(m_pStart[0], m_pStart[1], &ndc_x, &ndc_y);
            points = {ndc_x, ndc_y, 0.0f };
        }

    }
    else if (m_pStart[0] == m_pFinal[0]) // vertical line
    {
        int xvalue = m_pStart[0];
        int len = m_pStart[1] - m_pFinal[1];
        if (len < 0) // "upward"
        {
            int y = m_pStart[1];
            for (int i = 0; y < m_pFinal[1]; i++)
            {
                float ndc_x, ndc_y;
                this->convertToNDC(xvalue, y, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, 0.0f };
                points.insert(points.end(), point.begin(), point.end());
                y++;
            }
        }
        else if (len > 0) // "downward"
        {
            int y = m_pFinal[1];
            for (int i = 0; y < m_pStart[1]; i++)
            {
                float ndc_x, ndc_y;
                this->convertToNDC(xvalue, y, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, 0.0f };
                points.insert(points.end(), point.begin(), point.end());
                y++;
            }

        }
        else // m_pStart = m_pFinal
        {
            float ndc_x, ndc_y;
            this->convertToNDC(m_pStart[0], m_pStart[1], &ndc_x, &ndc_y);
            points = { ndc_x, ndc_y, 0.0f };
        }
    }
    // else // oblique line
    // {
    //     /* code */
    // }
    return points;
}