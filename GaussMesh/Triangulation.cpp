//
//  Triangulation.cpp
//  GaussMesh
//


#include "Triangulation.h"

#ifndef GLFW_DEFINED
#define GLFW_DEFINED
#include <GL/glfw.h>
#endif

using namespace std;

extern Mesh mesh;

//*****Comparator*****//
bool comparePointX(Point* p1, Point* p2)
{
	if(p1->p[0] < p2->p[0] || (p1->p[0] == p2->p[0] && p1->p[1] < p2->p[1]))
		return true;
	return false;
}

bool comparePointY(Point* p1, Point* p2)
{
	if(p1->p[1] < p2->p[1] || (p1->p[1] == p2->p[1] && p1->p[0] < p2->p[0]))
		return true;
	return false;
}

bool comparePointID(Point* p1, Point* p2)
{
	if(p1->id < p2->id)
		return true;
	return false;
}
//*****End Comparator*****//

//*****Calculating*****//
float length(float* p1, float* p2)
{
	float dx = p1[0] - p2[0];
	float dy = p1[1] - p2[1];
	float dz = p1[2] - p2[2];
	return sqrt(dx*dx + dy*dy + dz*dz);
}

float lengthSquare(float* p1, float* p2)
{
	float dx = p1[0] - p2[0];
	float dy = p1[1] - p2[1];
	float dz = p1[2] - p2[2];
	return dx*dx + dy*dy + dz*dz;
}

float orientation(float* center, float* point)
{
	// Set v = (0, 1, 0) be the base line
	// Map the angle between v and center-point from 0~360 to 0~1
	float dx = point[0] - center[0];
	float dy = point[1] - center[1];
	float len = sqrt(dx*dx + dy*dy);
	float cosTheta = dy / len;
	// cos: 0~180, 360~180 <=> 1~(-1)
	// map 1~(-1) to 0~0.5
	float orient = (1.0f - cosTheta) * 0.5f * 0.5f;
	if(dx < 0)
		orient = 1 - orient;
	return orient;
}

float circumcircle(float* p1, float* p2, float* p3, float* c)
{
	// Return radius
	float x1 = p1[0], x2 = p2[0], x3 = p3[0];
	float y1 = p1[1], y2 = p2[1], y3 = p3[1];
	float x11 = x1 * x1, x22 = x2 * x2, x33 = x3 * x3;
	float y11 = y1 * y1, y22 = y2 * y2, y33 = y3 * y3;
	float delta = 2 * (x1-x2) * (y3-y2) - 2 * (y1-y2) * (x3-x2);
	float dx = (y3-y2) * (x11+y11-x22-y22) - (y1-y2) * (x33+y33-x22-y22);
	float dy = (x1-x2) * (x33+y33-x22-y22) - (x3-x2) * (x11+y11-x22-y22);
	if(delta == 0)
		return -1;
	c[0] = dx / delta;
	c[1] = dy / delta;
	c[2] = 0;
	return length(p1, c);
}
//*****End Calculating*****//

//*****CDT Functions*****//
void link(Point* p1, Point* p2)
{
	// Add both of them into neighbor list
	Edge* edge = new Edge(p1, p2);
	edge->beLink();
	// Check the triangle candidate list
	// It is no matter we check the list of p1 or p2
	// If a candidate p' of p1 is linked to p2, it is also a candidate of p2, vice versa.
	list<NeighborLit>::iterator it;
	list<Triangle*> triList;
	for(it = p1->triCandidate.begin(); it != p1->triCandidate.end(); it++)
	{
		Edge* e2 = (*it)->e;
		Edge* e3 = (*it)->p->isLink(p2);
		if(e3 != NULL)
		{
			Triangle* tri = new Triangle(edge, e2, e3);
			triList.push_back(tri);
		}
	}
	list<Triangle*>::iterator tit;
	for(tit = triList.begin(); tit != triList.end(); tit++)
	{
		if((*tit)->beLink())// || (*tit)->midPoint[2] != 0)
			p1->parent->addTriangle(*tit);
		else
			delete *tit;
	}
}

bool linkTest(Point* p1, Point* p2)
{
	NeighborLit it;
	float theta = orientation(p1->p, p2->p);
	if(theta > p1->neighbor.back().theta && p1->neighbor.back().tR != NULL)
		return false;
	else
	{
		for(it = p1->neighbor.begin(); it != p1->neighbor.end(); it++)
		{
			if(it->theta >= theta)
			{
				if(it->tL != NULL || it->theta == theta)
					return false;
				// return true;
			}
            return true;
		}
	}
	// return true; // change to pass
}

NeighborLit examBaseCandidate(NeighborLit base, NeighborLit first, NeighborLit next, int LR)
{
	// LR = -1: left candidate; LR = 1: right candidate;
	Point *pbC = (base->e->p1 == base->p) ? base->e->p2 : base->e->p1;	// center point of base line
	Point *pbN = (base->e->p1 == base->p) ? base->e->p1 : base->e->p2;	// neighbor point of base line
	float theta = (first->theta < base->theta) ? first->theta - base->theta + 1 :
    first->theta - base->theta;
	float high = (LR == 1) ? 0.5f : 1.0f;
	float low = (LR == 1) ? 0 : 0.5f;
	while(theta < high && theta > low)
	{
		// Figure out if nCandiL is in circumcircle of b-f-n
		float center[3];
		float R = circumcircle(pbC->p, pbN->p, first->p->p, center);
		theta = (next->theta < base->theta) ? next->theta - base->theta + 1 :
        next->theta - base->theta;
		// If we find the candidate
		if((theta >= high || theta <= low) || lengthSquare(next->p->p, center) > R * R)
			return first;
		// Examine next candidate
		Edge* delEdge = first->e;
		first = next;
		next = loopIterator<Neighbor>(pbC->neighbor, next, LR);
		delEdge->unLink();
		delete delEdge;
	}
	return base;
}

void CDT(PointSet& ps)
{
	if(ps.pSet.size() == 2 || ps.pSet.size() == 3)
	{
		ps.sortPSet(Y);
		list<Point*>::iterator it = ps.pSet.begin();
		// link p1,p2 and p2,p3 if exist
		for(uint i = 0; i < ps.pSet.size() - 1; i++, it++)
		{
			list<Point*>::iterator next = it;
			link(*it, *(++next));
		}
		// link p3,p1
		if(ps.pSet.size() == 3 && linkTest(ps.pSet.front(), *it))
			link(ps.pSet.front(), *it);
        
		return;
	}
    
	PointSet ps1, ps2;
	ps.split(ps1, ps2);
	// Recursive call CDT into ps1, ps2
	CDT(ps1);
	CDT(ps2);
	// Merge two point set
	// Choose lowest point as baseB
    std::list<Point*>::iterator ps1it = ps1.pSet.begin();
    std::list<Point*>::iterator ps2it = ps2.pSet.begin();
    
    // printf("ps1 size : %ld, ps2 size : %ld\n", ps1.pSet.size(), ps2.pSet.size());
    
    
	Point *pbL = *ps1it;
	Point *pbR = *ps2it;
	bool acceptL = false, acceptR = false;
	bool clockwise = (pbL->p[1] < pbR->p[1]) ? false : true;
    
    // printf("pbL : %f, pbR : %f\n", pbL->p[1], pbR->p[1]);
    
	// Find the bottom line of two pointSet
    
	while(!(acceptL && acceptR))
	{
		// Find baseR along the convex hull
		Point* p1 = pbR;
		while(!acceptR)
		{
			Point* p2 = pbR->findConvexHull(clockwise, p1);
			if(!linkTest(pbR, pbL) || (p2 != NULL && orientation(pbL->p, pbR->p) < orientation(pbL->p, p2->p)))
			{
				p1 = pbR;
				pbR = p2;
				acceptL = false;
			}
			else
				acceptR = true;
		}
		// Find baseL along the convex hull
		p1 = pbL;
		while(!acceptL)
		{
			Point* p2 = pbL->findConvexHull(clockwise, p1);
			if(!linkTest(pbL, pbR) || (p2 != NULL && orientation(pbR->p, pbL->p) > orientation(pbR->p, p2->p)))
			{
				p1 = pbL;
				pbL = p2;
				acceptR = false;
			}
			else
				acceptL = true;
		}
	}
	link(pbL, pbR);
    

	// Link two point set
	while(1)
	{
		int score = 0;
		// find left candidate
		NeighborLit bCandiL = pbL->findNeighbor(pbR);
		NeighborLit fCandiL = loopIterator<Neighbor>(pbL->neighbor, bCandiL, -1);
		NeighborLit nCandiL = loopIterator<Neighbor>(pbL->neighbor, fCandiL, -1);
		if((fCandiL = examBaseCandidate(bCandiL, fCandiL, nCandiL, -1)) != bCandiL)
			score += 1;
		// find right candidate
		NeighborLit bCandiR = pbR->findNeighbor(pbL);
		NeighborLit fCandiR = loopIterator<Neighbor>(pbR->neighbor, bCandiR, 1);
		NeighborLit nCandiR = loopIterator<Neighbor>(pbR->neighbor, fCandiR, 1);
		if((fCandiR = examBaseCandidate(bCandiR, fCandiR, nCandiR, 1)) != bCandiR)
			score += 2;
		if(score == 0)
			break;
		// choose one candidate from left, right
		if(score == 1)
		{
			link(fCandiL->p, pbR);
			pbL = fCandiL->p;
		}
		else if(score == 2)
		{
			link(fCandiR->p, pbL);
			pbR = fCandiR->p;
		}
		else
		{
			float center[3];
			float R = circumcircle(pbR->p, pbL->p, fCandiL->p->p, center);
			if(lengthSquare(fCandiR->p->p, center) > R * R)
			{
				link(fCandiL->p, pbR);
				pbL = fCandiL->p;
			}
			else
			{
				link(fCandiR->p, pbL);
				pbR = fCandiR->p;
			}
		}
	}
	ps.merge(ps1, ps2);
}


void generalTrianglize(Point* p1, Point* p2, std::list<Point*> half, float theta12, float theta21)
{
	list<Point*>::iterator pit1 = half.begin();
	list<Point*>::reverse_iterator pit2 = half.rbegin();
	float thetaPit1 = orientation(p1->p, (*pit1)->p);
	float thetaPit2 = orientation(p2->p, (*pit2)->p);
	int n = half.size() - 1;
	pit1++;
	pit2++;
	// Link left half
	for(int i = 0; i < n;)
	{
		if(thetaPit1 != -1)
		{
			// Link if the edge is going to be linked is between pit1 and the constrainted edge
			float thetaPitN = orientation(p1->p, (*pit1)->p);
			if((thetaPitN-theta12) * (thetaPitN-thetaPit1) < 0)
			{
				link(p1, *pit1);
				pit1++;
				i++;
				thetaPit1 = thetaPitN;
			}
			else
				thetaPit1 = -1;
		}
		if(thetaPit2 != -1 && i < n)
		{
			// Link if the edge is going to be linked is between pit1 and the constrainted edge
			float thetaPitN = orientation(p2->p, (*pit2)->p);
			if((thetaPitN-theta21) * (thetaPitN-thetaPit2) < 0)
			{
				link(p2, *pit2);
				pit2++;
				i++;
				thetaPit2 = thetaPitN;
			}
			else
				thetaPit2 = -1;
		}
	}
}

void edgeConstraint(Point* p1, Point* p2)
{
	NeighborLit it = p1->neighbor.begin();
	float theta12 = orientation(p1->p, p2->p);
	float theta21 = (theta12 >= 0.5) ? theta12 - 0.5f : theta12 + 0.5f;
	Triangle* delTri = NULL;
	// Find the first triangle intersected by p1-p2
	if(theta12 > p1->neighbor.back().theta && p1->neighbor.back().tR != NULL)
		delTri = p1->neighbor.back().tR;
	else
	{
		for(it = p1->neighbor.begin(); it != p1->neighbor.end(); it++)
		{
			if(it->theta > theta12 && it->tL != NULL)
			{
				delTri = it->tL;
				break;
			}
		}
	}
	// Traverse p1-p2 and find out the triangles intersected by it
	// Record the edges to be unlink, and the points to be re-trianglized
	list<Point*> halfL, halfR;
	list<Edge*> eList;
	Edge* e = delTri->oppositeEdge(p1);
	// Push back the first two point into two halves
	float theta = orientation(p1->p, e->p1->p) - theta12;
	if(theta > 0.5f || (theta < 0 && theta > -0.5f))
	{
		halfL.push_back(e->p1);
		halfR.push_back(e->p2);
	}
	else
	{
		halfL.push_back(e->p2);
		halfR.push_back(e->p1);
	}
	// Push back the first edge
	eList.push_back(e);
	// Initial state: second triangle, first edge, second point(of halfL or halfR)
	delTri = (e->e1->tL == delTri) ? e->e1->tR : e->e1->tL;
	Point* p = delTri->oppositePoint(e);
	while(p != p2)
	{
		list<Point*> *pList;
		theta = orientation(p1->p, p->p) - theta12;
		if(theta > 0.5f || (theta < 0 && theta > -0.5f))
			pList = &halfL;
		else
			pList = &halfR;
		e = delTri->oppositeEdge(pList->back());
		delTri = (e->e1->tL == delTri) ? e->e1->tR : e->e1->tL;
		pList->push_back(p);
		eList.push_back(e);
		p = delTri->oppositePoint(e);
	}
	// Remove all the edges in the list
	list<Edge*>::iterator eit;
	for(eit = eList.begin(); eit != eList.end(); eit++)
	{
		(*eit)->unLink();
		delete *eit;
	}
	// Re-traianglize two halves
	generalTrianglize(p1, p2, halfL, theta12, theta21);
	generalTrianglize(p1, p2, halfR, theta12, theta21);
	// Link constrained edge
	link(p1, p2);
}

void boundConstraint(PointSet& ps)
{
	// Figure out the direction of hand drawing
	bool clock = ps.isClockwise();
    
	// Link the hand drawing lines
	list<Edge*> eList;
	list<Edge*>::iterator eit;
	list<Point*>::iterator first, second, prev;
    
	for(first = ps.pSet.begin(); first != ps.pSet.end(); first++)
	{
		second = loopIterator<Point*>(ps.pSet, first, 1);
		NeighborLit nei = (*first)->findNeighbor(*second);
		// If first doesn't link to second, then link them
		if(nei == (*first)->neighbor.end())
		{
			edgeConstraint(*first, *second);
			nei = (*first)->findNeighbor(*second);
		}
		eList.push_back(nei->e);
	}
	for(eit = eList.begin(); eit != eList.end(); eit++)
		(*eit)->setType(External);
    
	// Remove all edges ouside of the convex hull
	first = ps.pSet.begin();
	second = loopIterator<Point*>(ps.pSet, first, 1);
	prev = loopIterator<Point*>(ps.pSet, first, -1);
	int LR = (clock) ? -1 : 1;
	while(first != ps.pSet.end())
	{
		NeighborLit neiN = (*first)->findNeighbor(*second);
		NeighborLit neiP = (*first)->findNeighbor(*prev);
		neiN = loopIterator<Neighbor>((*first)->neighbor, neiN, LR);
		while(neiN != neiP)
		{
			Edge* e = neiN->e;
			neiN = loopIterator<Neighbor>((*first)->neighbor, neiN, LR);
			e->unLink();
			delete e;
		}
		prev = first;
		second = loopIterator<Point*>(ps.pSet, second, 1);
		first++;
	}
}
//*****End CDT*****//


//*****Bubble Up*****//
vector<Point*> getSpine(PointSet& ps, int baseID)
{
	vector<Point*> spine;
	
	ps.sortPSet(ID);
	list<Point*>::iterator pit;
	for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
	{
		if((*pit)->id >= baseID)
			spine.push_back(*pit);
	}
	return spine;
}

void cleanSpineBase(PointSet &ps, vector<Point*> spine)
{
	list<Point*>::iterator pit;
	int firstID = spine[0]->id;
	int lastID = spine.back()->id;
	for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
	{
		while((*pit)->id >= firstID && (*pit)->id <= lastID)
		{
			delete *pit;
			pit = ps.pSet.erase(pit);
		}
	}
}

void setAllPointNormal(PointSet &ps)
{
	// Calculate normal for all triangles
	// Calculate them last bacause we may refine the surface
	list<Triangle*>::iterator tit;
	for(tit = ps.triSet.begin(); tit != ps.triSet.end(); tit++)
		(*tit)->calNormal();
	// Calculate normal for all points
	list<Point*>::iterator pit;
	for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
		(*pit)->calNormal();
}
//*****End Bubble Up*****//


//*****Creat Spine*****//
int pruneAndSpine(PointSet &ps)
{
	int baseID = ps.pSet.size();
	vector<Point*> spine;
	list<Triangle*>::const_iterator it;
	vector<vector<Point*>> pListList;
	vector<vector<Edge*>> eListList;
	vector<Point*> endPointList;
	vector<Point*> toBeLinkList;
	vector<Triangle*> visitedJTriangleList;
	for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
		if((*it)->type == TRITYPE::T){		// start from T-triangle
			vector<Point*> pList;	// connect to end point
			vector<Edge*> eList;	// edges that need to be removed
			Point *endPoint = new Point(ps.pSet.size(), 0, 0, 0);
			float radius;
			Edge *currentEdge;
			Triangle* delTri = (*it);
			for(int i = 0; i < 3; i++)
				if((*it)->edge[i]->type == Internal){
					currentEdge = (*it)->edge[i];
					endPoint->p[0] = ((*it)->edge[i]->p1->p[0] + (*it)->edge[i]->p2->p[0]) / 2;
					endPoint->p[1] = ((*it)->edge[i]->p1->p[1] + (*it)->edge[i]->p2->p[1]) / 2;
					radius = pow(pow((*it)->edge[i]->p1->p[0] - (*it)->edge[i]->p2->p[0], 2) + pow((*it)->edge[i]->p1->p[1] - (*it)->edge[i]->p2->p[1], 2), 0.5f) / 2;
				}
			pList.push_back((*it)->oppositePoint(currentEdge));
			pList.push_back(currentEdge->p1);
			pList.push_back(currentEdge->p2);
			while(1){
				eList.push_back(currentEdge);
				bool stop = false;
				for(int i = 0; i < pList.size(); i++)	// stop condition, some vertex lies outside the semicircle
					if(pow(pow(endPoint->p[0] - pList[i]->p[0], 2) + pow(endPoint->p[1] - pList[i]->p[1], 2), 0.5f) > radius){
						stop = true;
						break;
					}
				delTri = (currentEdge->e1->tL == delTri) ? currentEdge->e1->tR : currentEdge->e1->tL;
				if(stop){
					for(int i = pList.size() - 1; i >= 0; i--)
						if(pList[i] == currentEdge->p1)
							pList.erase(pList.begin() + i);
					for(int i = pList.size() - 1; i >= 0; i--)
						if(pList[i] == currentEdge->p2)
							pList.erase(pList.begin() + i);
					eList.pop_back();
					break;
				}
				if(delTri->type == J){					// stop condition, advanced to junction triangle
					int i;
					for(i = 0; i < visitedJTriangleList.size(); i++)
						if(delTri == visitedJTriangleList[i])
							break;
					if(i == visitedJTriangleList.size())
						visitedJTriangleList.push_back(delTri);
					for(i = 0; i < spine.size(); i++)
						if(endPoint->p[0] == spine[i]->p[0] && endPoint->p[1] == spine[i]->p[1])
							break;
					Point *preEndPoint = new Point(endPoint->id, endPoint->p[0], endPoint->p[1], 0);
					for(int j = 0; j < 3; j++)
						endPoint->p[j] = delTri->midPoint[j];
					if(i != spine.size()){	// visit J triangle twice
						for(int i = pList.size() - 1; i >= 0; i--)
							if(pList[i] == currentEdge->p1)
								pList.erase(pList.begin() + i);
						for(int i = pList.size() - 1; i >= 0; i--)
							if(pList[i] == currentEdge->p2)
								pList.erase(pList.begin() + i);
						spine.erase(spine.begin() + i, spine.begin() + i + 2);
					} else{
						pList.push_back(delTri->oppositePoint(currentEdge));
						for(int i = 0; i < 3; i++)
							if(delTri->edge[i]->type == Internal && delTri->edge[i] != currentEdge){
								Point* tmp = new Point(ps.pSet.size(), 0, 0, 0);
								for(int j = 0; j < 3; j++)
									tmp->p[j] = (delTri->edge[i]->p1->p[j] + delTri->edge[i]->p2->p[j]) / 2;
								spine.push_back(tmp);
								spine.push_back(endPoint);
							}
					}
					break;
				} else{									// edge advancing
					int currentEdgeIndex;
					for(int i = 0; i < 3; i++)
						if(delTri->edge[i]->type == Internal){
							if(delTri->edge[i] != currentEdge){
								currentEdgeIndex = i;
								for(int j = 0; j < 3; j++)
									endPoint->p[j] = (delTri->edge[i]->p1->p[j] + delTri->edge[i]->p2->p[j]) / 2;
								radius = pow(pow(delTri->edge[i]->p1->p[0] - delTri->edge[i]->p2->p[0], 2) + pow(delTri->edge[i]->p1->p[1] - delTri->edge[i]->p2->p[1], 2), 0.5f) / 2;
							} else
								pList.push_back(delTri->oppositePoint(delTri->edge[i]));
						}
					currentEdge = delTri->edge[currentEdgeIndex];
				}
			}
			pListList.push_back(pList);
			eListList.push_back(eList);
			endPointList.push_back(endPoint);
		}
	}
	for(int i = 0; i < pListList.size(); i++){
		for(int j = 0; j < eListList[i].size(); j++)
			eListList[i][j]->unLink();
	}
	// handle the untouched triangles
	vector<Edge*> delEdgeList;
	for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
		if((*it)->type == TRITYPE::S){
			Point* tmp[2];
			tmp[0] = new Point(ps.pSet.size(), 0, 0, 0);
			tmp[1] = new Point(ps.pSet.size(), 0, 0, 0);
			int num = 0;
			for(int i = 0; i < 3; i++)
				if((*it)->edge[i]->type == Internal){
					tmp[num]->p[0] = ((*it)->edge[i]->p1->p[0] + (*it)->edge[i]->p2->p[0]) / 2;
					tmp[num]->p[1] = ((*it)->edge[i]->p1->p[1] + (*it)->edge[i]->p2->p[1]) / 2;
					int j;
					for(j = 0; j < delEdgeList.size(); j++)
						if((*it)->edge[i] == delEdgeList[j])
							break;
					if(j == delEdgeList.size()){	// untouched edge
						delEdgeList.push_back((*it)->edge[i]);
						toBeLinkList.push_back(tmp[num]);
						toBeLinkList.push_back((*it)->edge[i]->p1);
						toBeLinkList.push_back(tmp[num]);
						toBeLinkList.push_back((*it)->edge[i]->p2);
					}
					if(num == 0){
						toBeLinkList.push_back(tmp[num]);
						toBeLinkList.push_back((*it)->oppositePoint((*it)->edge[i]));
					}
					num++;
				}
			spine.push_back(tmp[0]);
			spine.push_back(tmp[1]);
		} else if((*it)->type == TRITYPE::J){
			int i;
			for(i = 0; i < visitedJTriangleList.size(); i++)
				if((*it) == visitedJTriangleList[i])
					break;
			if(i == visitedJTriangleList.size()){
				Point* tmp = new Point(ps.pSet.size(), 0, 0, 0);
				for(int j = 0; j < 3; j++)
					tmp->p[j] = (*it)->midPoint[j];
				vector<Point*> tmp_mid;
				tmp_mid.push_back(new Point(ps.pSet.size(), 0, 0, 0));
				tmp_mid.push_back(new Point(ps.pSet.size(), 0, 0, 0));
				tmp_mid.push_back(new Point(ps.pSet.size(), 0, 0, 0));
				int j;
				for(int a = 0; a < 3; a++){
					for(int j = 0; j < 3; j++)
						tmp_mid[a]->p[j] = ((*it)->edge[a]->p1->p[j] + (*it)->edge[a]->p2->p[j]) / 2;
					spine.push_back(tmp);
					spine.push_back(tmp_mid[a]);
					toBeLinkList.push_back(tmp);
					toBeLinkList.push_back((*it)->point[a]);
					for(j = 0; j < delEdgeList.size(); j++)
						if((*it)->edge[a] == delEdgeList[j])
							break;
					if(j == delEdgeList.size()){	// untouched edge
						delEdgeList.push_back((*it)->edge[a]);
						toBeLinkList.push_back(tmp_mid[a]);
						toBeLinkList.push_back((*it)->edge[a]->p1);
						toBeLinkList.push_back(tmp_mid[a]);
						toBeLinkList.push_back((*it)->edge[a]->p2);
					}
				}
			}
		}
	}
	for(int i = 0; i < delEdgeList.size(); i++)
		delEdgeList[i]->unLink();
	for(int i = 0; i < spine.size(); i += 2){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == spine[i]->p[0] && (*pit)->p[1] == spine[i]->p[1]){
				spine[i] = (*pit);
				break;
			}
		if(pit == ps.pSet.end()){
			spine[i]->id = ps.pSet.size();
			ps.addPoint(spine[i]);
		}
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == spine[i + 1]->p[0] && (*pit)->p[1] == spine[i + 1]->p[1]){
				spine[i + 1] = (*pit);
				break;
			}
		if(pit == ps.pSet.end()){
			spine[i + 1]->id = ps.pSet.size();
			ps.addPoint(spine[i + 1]);
		}
		link(spine[i], spine[i + 1]);
		NeighborLit it;
		for(it = spine[i]->neighbor.begin(); it != spine[i]->neighbor.end(); it++)
			it->e->setType(External);
	}
	for(int i = 0; i < toBeLinkList.size(); i += 2){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == toBeLinkList[i]->p[0] && (*pit)->p[1] == toBeLinkList[i]->p[1]){
				toBeLinkList[i] = (*pit);
				break;
			}
		link(toBeLinkList[i], toBeLinkList[i + 1]);
	}
	for(int i = 0; i < pListList.size(); i++){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == endPointList[i]->p[0] && (*pit)->p[1] == endPointList[i]->p[1])	// same end point in J triangle
				break;
		if(pit == ps.pSet.end()){
			endPointList[i]->id = ps.pSet.size();
			ps.addPoint(endPointList[i]);
			for(int j = 0; j < pListList[i].size(); j++)
				link(endPointList[i], pListList[i][j]);
		} else
			for(int j = 0; j < pListList[i].size(); j++)
				link((*pit), pListList[i][j]);
	}
	return baseID;
}