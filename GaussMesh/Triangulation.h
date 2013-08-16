//
//  Triangulation.h
//  GaussMesh
//


#ifndef GaussMesh_Triangulation_h
#define GaussMesh_Triangulation_h

#include <list>
#include <vector>
#include <map>

#ifndef GLEW_DEFINED
#define GLEW_DEFINED
#include <GL/glew.h>
#endif

#include "Mesh_structure.h"


//***Comparator***//
bool comparePointX(Point* p1, Point* p2);
bool comparePointY(Point* p1, Point* p2);
bool comparePointID(Point* p1, Point* p2);

//***Function for calculating***//
float length(float* p1, float* p2);
float lengthSquare(float* p1, float* p2);
float orientation(float* center, float* point);
float circumcircle(float* p1, float* p2, float* p3, float* c);

//***Function for CDT***//
void link(Point* p1, Point* p2);
bool linkTest(Point* pB, Point* pC);
void CDT(PointSet& ps);
void generalTrianglize(Point* p1, Point* p2, std::list<Point*> half, float theta12, float theta21);
void edgeConstraint(Point* p1, Point* p2);
void boundConstraint(PointSet& ps);
NeighborLit examBaseCandidate(NeighborLit base, NeighborLit first, NeighborLit second, int LR);

//***Header for Bubble up***//
std::vector<Point*> getSpine(PointSet &ps, int baseID);
void cleanSpineBase(PointSet &ps, std::vector<Point*> spine);
int pruneAndSpine(PointSet& ps);
void bubbleUp(PointSet& ps, std::vector<Point*>& spine, int upsideDown);
void setAllPointNormal(PointSet &ps);


template <class TClass>
typename std::list<TClass>::iterator loopIterator(std::list<TClass>& root, typename std::list<TClass>::iterator it, int s)
{
	if(it != --(root.end()) && s > 0)
		return ++it;
	else if(it != root.begin() && s < 0)
		return --it;
	else if(s > 0)
		return root.begin();
	return --(root.end());
}


#endif
