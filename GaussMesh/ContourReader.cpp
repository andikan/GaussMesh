//
//  ContourReader.cpp
//  GaussMesh
//


#include "ContourReader.h"


ContourReader::ContourReader(string filename)
{
    this->filename = filename;
    this->file.open(this->filename);
}

ContourReader::~ContourReader()
{
    this->filename = "";
    this->file.close();
}



vector<Point*> ContourReader::getContourPoints()
{
    char line[SIZE];
    vector<Point*> contourPoints;
    double xpos, ypos;
    int vertexNumber = 0;
    char* tmp;
    
    
    if(!this->file)
    {
        cout << "file cannot read : " << this->filename << endl;
    }
    else
    {
        file.getline(line, sizeof(line));
        cout << "contour number: " << line << endl;
        file.getline(line, sizeof(line));
        vertexNumber = atoi(line);
        vertexNumber = vertexNumber + 1;
        cout << "vertex number: " << vertexNumber << endl;
        
        for(int i=0; i<vertexNumber; i++)
        {
            file.getline(line, sizeof(line));
            
            if(line[0] == 'n')
            {
                continue;
            }
            else
            {
                if(i%2 == 0)
                {
                    tmp = strtok(line, " ");
                    xpos = atof(tmp);
                    tmp = strtok(NULL, " ");
                    ypos = atof(tmp);
                    // cout << "i : " << i+1 << ", x : " << xpos << ", y : " << ypos << endl;
                    
                    Point* point = new Point(i+1, -(xpos-202), -(ypos-182), 0.0);
                    contourPoints.push_back(point);
                }
            }
        }
    }
    
    return contourPoints;
}