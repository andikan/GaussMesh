//
//  Point.cpp
//  GaussMesh
//


#include "Mesh_structure.h"
#include "Triangulation.h"

using namespace std;

Point::Point(int _id, float x, float y, float z)
{
	p[0] = x; p[1] = y; p[2] = z;
	n[0] = n[1] = 0;
	n[2] = 1;
	this->id = _id;
}

Point::~Point()
{
	while(this->neighbor.size() != 0)
	{
		Neighbor nei = this->neighbor.front();
		nei.e->unLink();
		delete nei.e;
	}
	this->neighbor.clear();
	this->triCandidate.clear();
}

Edge* Point::isLink(const Point* p2)
{
	NeighborLit it;
	for(it = this->neighbor.begin(); it != this->neighbor.end(); it++)
	{
		if(it->p == p2)
			return it->e;
	}
	return NULL;
}

void Point::beAdd(PointSet* ps)
{
	this->parent = ps;
}

NeighborLit Point::addNeighbor(Edge* edge)
{
	Point* p2 = (edge->p1 == this) ? edge->p2 : edge->p1;
	// Calculate orient of p2 to this
	float theta = orientation(this->p, p2->p);
	// If the list is empty
	if(this->neighbor.empty())
	{
		this->neighbor.push_back(Neighbor(p2, edge, theta));
		this->triCandidate.push_back(this->neighbor.begin());
		return this->neighbor.begin();
	}
	// Insert to the list sorted by orient
	NeighborLit it;
	for(it = this->neighbor.begin(); it != this->neighbor.end(); it++)
	{
		if(theta < it->theta)
		{
			this->neighbor.insert(it, Neighbor(p2, edge, theta));
			break;
		}
	}
	if(it == this->neighbor.end())
		this->neighbor.push_back(Neighbor(p2, edge, theta));
	it--;	// Let it point to the Neighbor we insert
	this->triCandidate.push_back(it);
	return it;
}

bool Point::addTriNeighbor(Triangle* triangle)
{
    list<NeighborLit>::iterator nit, n[2];
	Edge *e1, *e2;
	Edge *opEdge = triangle->oppositeEdge(this);
	int k = 0;
	// Find corresponding triangle edge to the neighbor
	e1 = (opEdge == triangle->edge[0]) ? triangle->edge[1] : triangle->edge[0];
	e2 = (opEdge == triangle->edge[2]) ? triangle->edge[1] : triangle->edge[2];
	for(nit = this->triCandidate.begin(); nit != this->triCandidate.end(); nit++)
	{
		if((*nit)->e == e1)
			n[k++] = nit;
		else if((*nit)->e == e2)
			n[k++] = nit;
	}
	if(k != 2)
		return false;
	// Let n[1]->theta >= n[0]->theta
	if((*n[1])->theta < (*n[0])->theta)
	{
		nit = n[1];
		n[1] = n[0];
		n[0] = nit;
	}
	// Two cases: triangle in x-z plane and in 3d
	if(triangle->midPoint[2] == 0)
	{
		if((*n[1])->theta - (*n[0])->theta > 0.5 && (*n[0])->tL == NULL && (*n[1])->tR == NULL)
		{
			(*n[0])->tL = triangle;
			(*n[1])->tR = triangle;
		}
		else if((*n[1])->theta - (*n[0])->theta < 0.5 && (*n[0])->tR == NULL && (*n[1])->tL == NULL)
		{
			(*n[0])->tR = triangle;
			(*n[1])->tL = triangle;
		}
		else if((*n[1])->theta - (*n[0])->theta == 0.5)
			printf("Warning illegal triangle\n");
		else
			return false;
		// Handle triCandidate
		(*n[0])->ticket++;
		(*n[1])->ticket++;
		if((*n[0])->ticket == 2)
			this->triCandidate.erase(n[0]);
		if((*n[1])->ticket == 2)
			this->triCandidate.erase(n[1]);
		return true;
	}
	else
	{
		this->triNeighbor.push_back(triangle);
		return true;
	}
	printf("Warning illegal path\n");
	return true;
}

void Point::removeNeighbor(NeighborLit it)
{
	if(it->ticket < 2)
	{
		list<NeighborLit>::iterator nit;
		for(nit = this->triCandidate.begin(); nit != this->triCandidate.end(); nit++)
		{
			if(*nit == it)
			{
				this->triCandidate.erase(nit);
				break;
			}
		}
	}
	this->neighbor.erase(it);
}

void Point::removeTriangle(NeighborLit it, Triangle *tri)
{
	if(it->ticket == 2)
		this->triCandidate.push_back(it);
	it->ticket--;
	if(it->tL == tri)
		it->tL = NULL;
	else if(it->tR == tri)
		it->tR = NULL;
}

NeighborLit Point::findNeighbor(Point* point)
{
	NeighborLit it;
    int i = 0;
	for(it = this->neighbor.begin(); it != this->neighbor.end(); it++)
	{
        printf("neighbor: %d\n", i);
		if(it->p == point)
			break;
        i++;
	}
	return it;
}

NeighborLit Point::nextNeighbor(NeighborLit it, int d)
{
	if(it == this->neighbor.begin() && d < 0)
		return --(this->neighbor.end());
	else if(it == --(this->neighbor.end()) && d > 0)
		return this->neighbor.begin();
	else if(d > 0)
		return ++it;
	return --it;
}

Point* Point::findConvexHull(bool clockwise, const Point* pp)
{
    if(this->neighbor.size() == 1)
        return this->neighbor.begin()->p;
    else if(this->neighbor.size() == 0)
        return NULL;
    
	NeighborLit first, second;
	second = ++(this->neighbor.begin());
	for(first = this->neighbor.begin(); second != this->neighbor.end(); first++, second++)
	{
		if(second->theta - first->theta >= 0.5f)
			return (clockwise && second->p != pp) ? second->p : first->p;
	}
	second = this->neighbor.begin();
	float theta = second->theta - first->theta;
	if(theta >= 0.5f || (theta <= 0 && theta + 1 >= 0.5f))
		return (clockwise && second->p != pp) ? second->p : first->p;
	return NULL;
}

void Point::calNormal()
{
	list<Triangle*>::iterator tit;
	this->n[2] = 0;
	for(tit = this->triNeighbor.begin(); tit != this->triNeighbor.end(); tit++)
	{
		for(int i = 0; i < 3; i++)
			this->n[i] += (*tit)->normal[i];
	}
	float length = sqrt(this->n[0] * this->n[0] + this->n[1] * this->n[1] + this->n[2] * this->n[2]);
	for(int i = 0; i < 3; i++)
		this->n[i] /= length;
}

bool Point::isEqualTo(const Point* p2)
{
    if(this->p[0] == p2->p[0] && this->p[1] == p2->p[1] && this->p[2] == p2->p[2])
        return true;
    else
        return false;
}