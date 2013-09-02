//
//  Mesh_structure.h
//  GaussMesh
//
//  Created by  Andikan on 13/8/6.
//  Copyright (c) 2013 Andikan. All rights reserved.
//

#ifndef GaussMesh_Mesh_structure_h
#define GaussMesh_Mesh_structure_h

#include <list>
#include <vector>
#include <map>

#ifndef GLEW_DEFINED
#define GLEW_DEFINED
#include <GL/glew.h>
#endif

// Include GLM
#include <glm/glm.hpp>

// Include poly2tri
#include "poly2tri/poly2tri.h"

struct Neighbor;
class Point;
class Edge;
class Triangle;
class PointSet;

typedef std::list<Neighbor>::iterator NeighborLit;
typedef unsigned int uint;


struct Neighbor
{
public:
    Neighbor(Point* point, Edge* edge, float _theta):p(point), e(edge), theta(_theta)
    {
        tL = NULL;
        tR = NULL;
        ticket = 0;
    }
    Point* p;
    Edge* e;
    Triangle* tL, *tR;
    float theta;
    int ticket;
};

class Point
{
public:
    Point(int _id, float x, float y, float z);
    ~Point();
    Edge* isLink(const Point* p2);
    void beAdd(PointSet* ps);
    NeighborLit addNeighbor(Edge* edge);
    bool addTriNeighbor(Triangle* triangle);
    void removeNeighbor(NeighborLit it);
    void removeTriangle(NeighborLit it, Triangle *tri);
    
    NeighborLit findNeighbor(Point* point);
    NeighborLit nextNeighbor(NeighborLit it, int d);
    Point* findConvexHull(bool clockwise, const Point* pp);
    void calNormal();
    
    float p[3];
    float n[3];
    int id;
    PointSet* parent;
    std::list<Neighbor> neighbor;	  // clockwise
    std::list<NeighborLit> triCandidate;
    std::list<Triangle*> triNeighbor;
};

enum EDGETYPE {Internal, External};
class Edge
{
public:
    Edge(Point* _p1, Point* _p2);
    void beLink();
    void unLink();
    void setType(int _type);
    
    int type;
    Point *p1, *p2;
    NeighborLit e1, e2;	// e1: iterator of p1; e2: iterator of p2
};

enum TRITYPE {J, S, T};
// J: no external edge S: one external edge; T: two external edge;
class Triangle
{
public:
    Triangle(Edge* e1, Edge* e2, Edge* e3);
    bool beLink();
    void unLink();
    void remove(Edge *e);
    void beAdd(PointSet *ps, std::list<Triangle*>::iterator it);
    void vote(int score);
    Edge* oppositeEdge(Point* p);
    Point* oppositePoint(Edge* e);
    void calNormal();
    
    int type;
    Point* point[3];
    Edge* edge[3];
    float midPoint[3];
    float normal[3];
    PointSet* parent;
    std::list<Triangle*>::iterator tit;
};

enum SORTTYPE {X, Y, ID};
class PointSet
{
public:
    PointSet();
    ~PointSet();
    void split(PointSet& ps1, PointSet& ps2);
    void merge(PointSet& ps1, PointSet& ps2);
    void clear();
    void release();
    void sortPSet(int sort);		// 0: sort by x; 1: sort by y;
    void addPoint(Point* p);
    void addTriangle(Triangle* t);
    
    bool isClockwise();
    
    std::list<Point*> pSet;
    std::list<Triangle*> triSet;
};


enum MERGETYPE {NONE, JJ, JSJ, PREJSJ, JJJ};
class Mesh
{
public:
    GLuint pNum;
    GLuint VBid;
    GLfloat *vertexBuffer;
    GLuint assm;
    GLenum drawMode;
    PointSet* ps;
    
    void loadEdgeFromMouse(std::vector<glm::vec3> point);
    void loadAndSortPointSet();
    void delaunayTriangulation();
    void constraintBound();
    std::vector<Point*> getSpinePoints(PointSet &ps);
    std::vector<Point*> getJointTriangleSpinePoints(PointSet &ps);
    
    void loadP2tPoints(std::vector<p2t::Point*> polyline);
    void addP2tTriangles(std::vector<p2t::Triangle*> triangles);
    std::vector<Point*> getSkeletonPointSet(PointSet &ps);
    void findNextSkeletonPoint(std::vector<Point*> &skeletonPoints, Triangle* currentTriangle, Triangle* prevTriangle, Triangle* prevJointTriangle, int prevMergeType);
    
    
    void loadTriangleFromPointSet(const PointSet& ps);
    void loadEdgeFromPointSet(const PointSet& ps);
    void runCDT();
    void release();
    
    Mesh();
    ~Mesh();
};


#endif
