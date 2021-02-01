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

    if (m_pStart[0] == m_pFinal[0] && m_pStart[1] == m_pFinal[1]) // a single point
    {
        float ndc_x, ndc_y;
        this->convertToNDC(m_pStart[0], m_pStart[1], &ndc_x, &ndc_y);
        points = { ndc_x, ndc_y, 0.0f };
    }
    else if (m_pStart[1] == m_pFinal[1]) // horizontal line
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
    }
    // TODO: implement oblique line (case 5-16)
    // else if (True)
    // {
    //     /* code */
    // }
    return points;
}

std::vector<float> Line::createDottedPoints(const unsigned int _pattern)
{
    unsigned int curr_pattern;
    std::vector<float> points = {};

    if (m_pStart[0] == m_pFinal[0] && m_pStart[1] == m_pFinal[1]) // a single point
    {
        float ndc_x, ndc_y;
        this->convertToNDC(m_pStart[0], m_pStart[1], &ndc_x, &ndc_y);
        points = { ndc_x, ndc_y, 0.0f };
    }
    else if (m_pStart[1] == m_pFinal[1]) // horizontal line
    {
        int yvalue = m_pStart[1];
        int len = m_pStart[0] - m_pFinal[0];
        int counter = 0;
        curr_pattern = _pattern;
        if (len < 0) // "rightward"
        {
            int x = m_pStart[0];
            for (int i = 0; x < m_pFinal[0]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                this->convertToNDC(x, yvalue, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, ndc_z};
                points.insert(points.end(), point.begin(), point.end());
                x++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
        else if (len > 0) // "leftward"
        {
            int x = m_pFinal[0];
            for (int i = 0; x < m_pStart[0]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                this->convertToNDC(x, yvalue, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                points.insert(points.end(), point.begin(), point.end());
                x++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
    }
    else if (m_pStart[0] == m_pFinal[0]) // vertical line
    {
        int xvalue = m_pStart[0];
        int len = m_pStart[1] - m_pFinal[1];
        int counter = 0;
        curr_pattern = _pattern;
        if (len < 0) // "upward"
        {
            int y = m_pStart[1];
            for (int i = 0; y < m_pFinal[1]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                this->convertToNDC(xvalue, y, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                points.insert(points.end(), point.begin(), point.end());
                y++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
        else if (len > 0) // "downward"
        {
            int y = m_pFinal[1];
            for (int i = 0; y < m_pStart[1]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                this->convertToNDC(xvalue, y, &ndc_x, &ndc_y);
                std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                points.insert(points.end(), point.begin(), point.end());
                y++;

                if (counter == 23)
                {
                    counter = 0;
                    curr_pattern = _pattern;
                }
                else
                {
                    counter++;
                    // insert a zero on the left and push every bit to the right, and the rightmost will be thrown away.
                    // that was not a very technical description for a right bit shift of 1.
                    curr_pattern = curr_pattern >> 1;
                }
            }
        }
    }
    // TODO: implement oblique line (case 5-16)
    // else if (True)
    // {
    //     /* code */
    // }
    return points;
}