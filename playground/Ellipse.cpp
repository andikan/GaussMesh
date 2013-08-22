#include "CDT.h"
#include "Ellipse.h"

#define M_PI 3.14159265358979323846

using namespace std;

bool circumDivide(float theta1, float theta2, EData& data)
{
	float p1[2] = {data.a * cos(theta1), data.b * sin(theta1)};
	float p2[2] = {data.a * cos(theta2), data.b * sin(theta2)};
	float dlen = length(p1, p2) - data.step;

	if(dlen <= data.step)
	{
		// Check if previous dlen is better
		if(dlen * 2 < data.pdlen)
		{
			data.tList.push_back(theta2);
			return true;
		}
		return false;
	}
	else
	{
		float theta = theta1 + 0.5f*(theta2-theta1);
		data.pdlen = dlen;
		if(!circumDivide(theta1, theta, data))
		{
			data.tList.push_back(theta2);
			return true;
		}
		if(!circumDivide(theta, theta2, data))
			data.tList.push_back(theta2);
	}
	return true;
}

vector<Point*> circumPoint(PointSet& ps, Point* base, Point* top, Point* side)
{
	// x^2 / a + y^2 / b = 1
	float axisA[3], axisB[3];
	for(int i = 0; i < 3; i++)
	{
		axisA[i] = top->p[i] - base->p[i];
		axisB[i] = side->p[i] - base->p[i];
	}
	float a = length(top->p, base->p);
	float b = length(side->p, base->p);
	float len = (b < a) ? 0.5f*M_PI*b + a - b : 0.5f*M_PI*a + b - a;
	float step = len * 0.25f;
	EData data(a, b, step);
	circumDivide(0, 0.5f*M_PI, data);

	// Build the curve
	int n = data.tList.size()-1;
	vector<Point*> curve;
	curve.push_back(top);
	for(int i = 0; i < n; i++)
	{
		float c[2] = {cos(data.tList[i]), sin(data.tList[i])}, p[3];
		for(int j = 0; j < 3; j++)
			p[j] = base->p[j] + c[0]*axisA[j] + c[1]*axisB[j];
		Point* np = new Point(ps.pSet.size(), p[0], p[1], p[2]);
		ps.addPoint(np);
		curve.push_back(np);
		link(curve[i], np);
	}
	// Don't need to create last point on curve because we already have one(side)
	link(curve.back(), side);
	curve.push_back(side);
	return curve;
}

float avgNeighborLength(Point* base)
{
	NeighborLit it;
	float len = 0;
	for(it = base->neighbor.begin(); it != base->neighbor.end(); it++)
	{
		if(it->p->id < base->id)
			len += length(base->p, it->p->p);
	}
	return len / base->neighbor.size();
}

NeighborLit findNextOutsideNeighbor(Point* &base1, NeighborLit start, int baseID)
{
	// Get iterator follow start
	start = loopIterator<Neighbor>(base1->neighbor, start, 1);
	while(start->p->id >= baseID)
	{
		// If we meet next base
		Point* base2;
		base2 = start->p;
		start = (start->e->p1 == base1) ? start->e->e2 : start->e->e1;
		base1 = base2;
		start = loopIterator<Neighbor>(base1->neighbor, start, 1);
	}
	return start;
}

void sew2Curve(vector<Point*> curve1, vector<Point*> curve2)
{
	int nLong, nShort, count;
	bool big = (curve1.size() > curve2.size());
	// Initialize variable as a longer curve and shorter one
	nLong = (big) ? curve1.size()-1 : curve2.size()-1;	// There is one pair of point don't need to be linked
	nShort = (big) ? curve2.size()-1 : curve1.size()-1;	// The heads or the tails
	count = (int)ceil((float)nLong/nShort);
	bool head = (curve1.front() == curve2.front());
	int i = (head) ? 1 : nLong-1;
	int j = (head) ? 1 : nShort-1;
	int &c1 = (big) ? i : j;
	int &c2 = (big) ? j : i;
	int step = (head) ? 1 : -1;
	int tick = 0;
	
	// Sew two curve together
	while((i != 0 || j != 0) && (i != nLong || j != nShort))
	{
		if(tick < count)
		{
			link(curve2[c2], curve1[c1]);
			tick++;
			i += step;
			if(i < 0 || i > nLong)
				break;
		}
		else
		{
			link(curve2[c2], curve1[c1]);
			j += step;
			tick = 0;
			if(j < 0 || j > nShort)
				break;
		}
	}
	// Cover bug for missing outside edge
	if(curve2.back() != curve1.back() && !(curve2.back()->isLink(curve1.back())))
		link(curve2.back(), curve1.back());
}

void linkSmoothSpine(vector<pair<Point*, Point*> > &spine)
{
	vector<SpinePoint> spineSet;
	int baseID = spine[0].first->id;
	Point *base1, *base2, *start1, *start2;
	NeighborLit nit;
	
	// Build spineSet
	for(uint i = 0; i < spine.size(); i++)
		spineSet.push_back(SpinePoint(spine[i].first, spine[i].second));
	// Find next base of base1
	base1 = spineSet[0].base;
	for(nit = base1->neighbor.begin(); nit != base1->neighbor.end(); nit++)
	{
		if(nit->p->id >= baseID)
		{
			base2 = nit->p;
			spineSet[base1->id-baseID].neighbor.push_back(&spineSet[base2->id-baseID]);
			spineSet[base2->id-baseID].neighbor.push_back(&spineSet[base1->id-baseID]);
			link(spineSet[base1->id-baseID].top, spineSet[base2->id-baseID].top);
			break;
		}
	}
	start1 = base1;
	start2 = base2;
	// Start tracing spine line
	do
	{
		// Turn around the neighbor iterator
		nit = (nit->e->p1 == base1) ? nit->e->e2 : nit->e->e1;
		// Swap base
		base1 = base2;
		nit = loopIterator<Neighbor>(base1->neighbor, nit, 1);
		while(nit->p->id < baseID)
			nit = loopIterator<Neighbor>(base1->neighbor, nit, 1);
		base2 = nit->p;
		if(!(spineSet[base1->id-baseID].isNeighbor(base2)))
		{
			spineSet[base1->id-baseID].neighbor.push_back(&spineSet[base2->id-baseID]);
			spineSet[base2->id-baseID].neighbor.push_back(&spineSet[base1->id-baseID]);
			link(spineSet[base1->id-baseID].top, spineSet[base2->id-baseID].top);
		}
	}while(start1 != base1 || start2 != base2);
	// Smooth the height with 1D-gaussian filter: 1/sqrt(2*PI) * e^(-x^2 / 2)
	float sigma = 0.25;
	float invSqrt2PI = 1.0f / (sigma * 2 * M_PI);
	float invSqrte = exp(-0.5f/(sigma*sigma));
	for(uint i = 0; i < spineSet.size(); i++)
	{
		vector<Point*> nTop;
		if(spineSet[i].neighbor.size() == 1)
			continue;
		for(uint j = 0; j < spineSet[i].neighbor.size(); j++)
		{
			SpinePoint *nsp = spineSet[i].neighbor[j];
			for(uint k = 0; k < nsp->neighbor.size(); k++)
			{
				if(nsp->neighbor[k] != &spineSet[i])
					nTop.push_back(nsp->neighbor[k]->top);
			}
			nTop.push_back(nsp->top);
		}
		Point* center = spineSet[i].top;
		float sumW = 1;
		float sumH = center->p[2];
		for(uint j = 0; j < nTop.size(); j++)
		{
			float lenS = lengthSquare(center->p, nTop[j]->p);
			float weight = invSqrt2PI * pow(invSqrte, lenS);
			sumW += weight;
			sumH += weight * nTop[j]->p[2];
		}
		center->p[2] = sumH / sumW;
	}
}
