#include "CDT.h"

using namespace std;

PointSet::PointSet()
{
}

PointSet::~PointSet()
{
	list<Point*>::iterator pit;
	for(pit = this->pSet.begin(); pit != this->pSet.end(); pit++)
		delete *pit;
	this->pSet.clear();
	// delete point => delete edges connect to point => delete triangle form by edges
	this->triSet.clear();
}

void PointSet::release()
{
	list<Point*>::iterator pit;
	for(pit = this->pSet.begin(); pit != this->pSet.end(); pit++)
		delete *pit;
	this->pSet.clear();
	// delete point => delete edges connect to point => delete triangle form by edges
	this->triSet.clear();
}

void PointSet::sortPSet(int sort)
{
	// Sort by x
	if(sort == X)
		this->pSet.sort(comparePointX);
	// Sort by y
	else if(sort == Y)
		this->pSet.sort(comparePointY);
	// Sort by id
	else if(sort == ID)
		this->pSet.sort(comparePointID);
}

void PointSet::addPoint(Point* point)
{
	this->pSet.push_back(point);
	point->beAdd(this);
}

void PointSet::addTriangle(Triangle* triangle)
{
	this->triSet.push_back(triangle);
	triangle->beAdd(this, --(this->triSet.end()));
}

void PointSet::split(PointSet& ps1, PointSet& ps2)
{
	int n1 = this->pSet.size() * 0.5;
	int n2 = this->pSet.size() - n1;
	int i = 0;
	list<Point*>::iterator it;
	for(it = this->pSet.begin(); i < n1; it++, i++)
		ps1.addPoint(*it);
	for(i = 0; i < n2; it++, i++)
		ps2.addPoint(*it);
}

void PointSet::merge(PointSet& ps1, PointSet& ps2)
{
	list<Point*>::iterator pit1 = ps1.pSet.begin();
	list<Point*>::iterator pit2 = ps2.pSet.begin();
	int n = ps1.pSet.size() + ps2.pSet.size();
	
	// Merge points by y sorted
	this->pSet.clear();
	for(int i = 0; i < n; i++)
	{
		list<Point*>::iterator *ppit;
		if(pit1 == ps1.pSet.end())
			ppit = &pit2;
		else if(pit2 == ps2.pSet.end())
			ppit = &pit1;
		else
			ppit = ((*pit1)->p[1] < (*pit2)->p[1]) ? &pit1 : &pit2;
		this->addPoint(**ppit);
		(*ppit)++;
	}
	// Merge triangles
	list<Triangle*>::iterator tit1, tit2;
	for(tit1 = ps1.triSet.begin(); tit1 != ps1.triSet.end(); tit1++)
		this->addTriangle(*tit1);
	for(tit2 = ps2.triSet.begin(); tit2 != ps2.triSet.end(); tit2++)
		this->addTriangle(*tit2);
	// Clear ps1, ps2
	ps1.clear();
	ps2.clear();
}

void PointSet::clear()
{
	this->pSet.clear();
	this->triSet.clear();
}

bool PointSet::isClockwise()
{
	this->sortPSet(ID);
	float theta = 0;
	list<Point*>::iterator first, second, third;
	first = this->pSet.begin();
	second = loopIterator<Point*>(this->pSet, first, 1);
	third = loopIterator<Point*>(this->pSet, second, 1);
	for(uint i = 0; i < this->pSet.size(); i++)
	{
		float dTheta = orientation((*second)->p, (*third)->p) - orientation((*first)->p, (*second)->p);
		if(dTheta > 0.5)
			dTheta -= 1;
		else if(dTheta < -0.5)
			dTheta += 1;
		theta += dTheta;
		first = second; second = third;
		third = loopIterator<Point*>(this->pSet, third, 1);
	}
	return (theta > 0);
}