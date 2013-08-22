struct EData
{
	EData(float _a, float _b, float _step):a(_a), b(_b), step(_step) {}
	float a, b;
	float step;
	float pdlen;
	std::vector<float> tList;
};

class SpinePoint
{
	public:
		SpinePoint(Point* _base, Point* _top):base(_base), top(_top) {}
		bool isNeighbor(Point* npoint)
		{
			for(uint i = 0; i < this->neighbor.size(); i++)
			{
				if(this->neighbor[i]->base == npoint)
					return true;
			}
			return false;
		}
		Point* base;
		Point* top;
		std::vector<SpinePoint*> neighbor;
};


bool circumDivide(float theta1, float theta2, EData *data);
std::vector<Point*> circumPoint(PointSet& ps, Point* base, Point* top, Point* side);
float avgNeighborLength(Point* base);
void sew2Curve(std::vector<Point*> curve1, std::vector<Point*> curve2);
NeighborLit findNextOutsideNeighbor(Point* &base1, NeighborLit start, int baseID);
void linkSmoothSpine(std::vector<std::pair<Point*, Point*> > &spineSet);
