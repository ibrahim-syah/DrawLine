#include "Line.h"

Line::Line()
{
}

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
    // convert into opengl Normalized Device Coordinates (NDC) range (-1 to 1)
    *r_x = 2.0f * _x / m_width - 1.0f;
    *r_y = 2.0f * _y / m_height - 1.0f;
}


// This code is very redundant, there's a lot of code duplication, it's useful to get a 
// solid understanding of how the algorithm works, but be sure to fix this spaghetti code
// before shipping it
std::vector<float> Line::createPoints(const unsigned int _pattern, const int _lineWidth)
{
    unsigned int curr_pattern;
    std::vector<float> points = {};

    int counter = 0;
    curr_pattern = _pattern;

    if (m_pStart[0] == m_pFinal[0] && m_pStart[1] == m_pFinal[1]) // a single point
    {
        float ndc_x, ndc_y;
        this->convertToNDC(m_pStart[0], m_pStart[1], &ndc_x, &ndc_y);
        points = { ndc_x, ndc_y, 0.0f };
        std::cout << "case 0 (point)" << std::endl;
    }
    else if (m_pStart[1] == m_pFinal[1]) // horizontal line
    {
        int yvalue = m_pStart[1];
        int len = m_pStart[0] - m_pFinal[0];
        int counter = 0;
        curr_pattern = _pattern;
        if (len < 0) // "rightward"
        {
            std::cout << "case 1 (horizontal right)" << std::endl;
            int x = m_pStart[0];
            for (int i = 0; x < m_pFinal[0]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(x, yvalue+j, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
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
            std::cout << "case 3 (horizontal left)" << std::endl;
            int x = m_pFinal[0];
            for (int i = 0; x < m_pStart[0]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(x, yvalue + j, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
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
            std::cout << "case 2 (vertical up)" << std::endl;
            int y = m_pStart[1];
            for (int i = 0; y < m_pFinal[1]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(xvalue + j, y, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
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
            std::cout << "case 4 (vertical down)" << std::endl;
            int y = m_pFinal[1];
            for (int i = 0; y < m_pStart[1]; i++)
            {
                float ndc_x, ndc_y, ndc_z;
                ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                for (int j = 0; j < _lineWidth; j++)
                {
                    this->convertToNDC(xvalue + j, y, &ndc_x, &ndc_y);
                    std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                    points.insert(points.end(), point.begin(), point.end());
                }
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
    else if (std::abs(m_pFinal[0] - m_pStart[0]) == std::abs(m_pFinal[1] - m_pStart[1]) ) // 45 degree cases
    {
        int len = m_pStart[0] - m_pFinal[0];

        int x = m_pStart[0];
        int y = m_pStart[1];
        if (len < 0) // rightward
        {
            if (m_pFinal[1] > m_pStart[1]) // upward rightward
            {
                std::cout << "case 5 (45 degree 1st quadrant)" << std::endl;
                for (int i = 0; x < m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y+j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x++;
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
            else // rightward downward
            {
                std::cout << "case 8 (45 degree 4th quadrant)" << std::endl;
                for (int i = 0; x < m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x++;
                    y--;

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
        else if (len > 0) // leftward
        {
            if (m_pFinal[1] > m_pStart[1]) // upward leftward
            {
                std::cout << "case 6 (45 degree 2nd quadrant)" << std::endl;
                for (int i = 0; x > m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x--;
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
            else // leftward downward
            {
                std::cout << "case 7 (45 degree 3rd quadrant)" << std::endl;
                for (int i = 0; x > m_pFinal[0]; i++)
                {
                    float ndc_x, ndc_y, ndc_z;
                    ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                    for (int j = 0; j < _lineWidth; j++)
                    {
                        this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                        std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                        points.insert(points.end(), point.begin(), point.end());
                    }
                    x--;
                    y--;

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
    }
    else // case 9 - 16 with bresenham
    {
        int dx = abs(m_pFinal[0] - m_pStart[0]);
        int dy = abs(m_pFinal[1] - m_pStart[1]);

        if (dx > dy)
        {
            if (m_pFinal[0] > m_pStart[0])
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 9 (theta < 45 degree 1st quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x < m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y++;
                        }

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
                else
                {
                    std::cout << "case 16 (theta < 45 degree 4th quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x < m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y--;
                        }

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
            else
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 12 (theta < 45 degree 2nd quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x > m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y++;
                        }

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
                else
                {
                    std::cout << "case 13 (theta < 45 degree 3rd quadrant)" << std::endl;

                    int dR = 2 * dy;
                    int dUR = 2 * (dy - dx);
                    int d = 2 * dy - dx;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; x > m_pFinal[0]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x, y + j, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        x--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            y--;
                        }

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
            
        } // |dy| > |dx|
        else
        {
            if (m_pFinal[0] > m_pStart[0])
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 10 (theta > 45 degree 1st quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y < m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x++;
                        }

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
                else
                {
                    std::cout << "case 15 (theta > 45 degree 4th quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y > m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x++;
                        }

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
            else
            {
                if (m_pFinal[1] > m_pStart[1])
                {
                    std::cout << "case 11 (theta > 45 degree 2nd quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y < m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y++;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x--;
                        }

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
                else
                {
                    std::cout << "case 14 (theta > 45 degree 3rd quadrant)" << std::endl;
                    int dR = 2 * dx;
                    int dUR = 2 * (dx - dy);
                    int d = 2 * dx - dy;

                    int x = m_pStart[0];
                    int y = m_pStart[1];
                    for (int i = 0; y > m_pFinal[1]; i++)
                    {
                        float ndc_x, ndc_y, ndc_z;
                        ndc_z = (curr_pattern & 0x000001) ? 0.0f : 2.0f; // if it maps to a pattern index of 0, set it outside of the accepted z coordinate

                        for (int j = 0; j < _lineWidth; j++)
                        {
                            this->convertToNDC(x + j, y, &ndc_x, &ndc_y);
                            std::vector<float> point = { ndc_x, ndc_y, ndc_z };
                            points.insert(points.end(), point.begin(), point.end());
                        }
                        y--;

                        if (d < 0) // M is below the line, pick R
                        {
                            d = d + dR;
                        }
                        else // M is either on the line or above the line, either case pick UR
                        {
                            d = d + dUR;
                            x--;
                        }

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
        }
    }
    return points;
}

void Line::writeJSON(const std::string filename, const float point_size, const int pattern, const float clear_color[4], const float line_color[4], const int lineWidth)
{
    m_root["scr_width"] = m_width;
    m_root["scr_height"] = m_height;

    m_root["pStart"]["x"] = m_pStart[0];
    m_root["pStart"]["y"] = m_pStart[1];
    m_root["pFinal"]["x"] = m_pFinal[0];
    m_root["pFinal"]["y"] = m_pFinal[1];

    m_root["line_width"] = lineWidth;

    m_root["point_size"] = point_size;
    m_root["spacing_index"] = pattern;

    m_root["clear_color"]["R"] = clear_color[0];
    m_root["clear_color"]["G"] = clear_color[1];
    m_root["clear_color"]["B"] = clear_color[2];
    m_root["clear_color"]["A"] = clear_color[3];

    m_root["line_color"]["R"] = line_color[0];
    m_root["line_color"]["G"] = line_color[1];
    m_root["line_color"]["B"] = line_color[2];
    m_root["line_color"]["A"] = line_color[3];

    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "\t";
    std::string document = Json::writeString(wbuilder, m_root);

    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    if (!file) {
        std::cout << "File not created!" << std::endl;
    }
    else {
        std::cout << "File created successfully!" << std::endl;
        file << document << "\n";
        file.close();
    }
}

bool Line::readJSON(const std::string filename, int pStart[2], int pFinal[2], float *point_size, int *pattern, float clear_color[4], float line_color[4], int *lineWidth)
{
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;

    std::ifstream file;
    file.open(filename.c_str());
    if (!file) {
        std::cout << "File cannot be read!" << std::endl;
        return false;
    }
    else {
        std::cout << "Read file successfully!" << std::endl;
        bool ok = Json::parseFromStream(rbuilder, file, &m_root, &errs);

        m_width = m_root["scr_width"].asInt();
        m_height = m_root["scr_height"].asInt();
        m_pStart[0] = m_root["pStart"]["x"].asInt();
        m_pStart[1] = m_root["pStart"]["y"].asInt();
        m_pFinal[0] = m_root["pFinal"]["x"].asInt();
        m_pFinal[1] = m_root["pFinal"]["y"].asInt();

        pStart[0] = m_pStart[0];
        pStart[1] = m_pStart[1];
        pFinal[0] = m_pFinal[0];
        pFinal[1] = m_pFinal[1];

        *lineWidth = m_root["line_width"].asInt();

        *point_size = m_root["point_size"].asFloat();
        *pattern = m_root["spacing_index"].asInt();

        clear_color[0] = m_root["clear_color"]["R"].asFloat();
        clear_color[1] = m_root["clear_color"]["G"].asFloat();
        clear_color[2] = m_root["clear_color"]["B"].asFloat();
        clear_color[3] = m_root["clear_color"]["A"].asFloat();

        line_color[0] = m_root["line_color"]["R"].asFloat();
        line_color[1] = m_root["line_color"]["G"].asFloat();
        line_color[2] = m_root["line_color"]["B"].asFloat();
        line_color[3] = m_root["line_color"]["A"].asFloat();

        file.close();
        return true;
    }
}