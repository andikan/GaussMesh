#include <iostream>
#include <vector>

using namespace std;

class Triangle{
public:
	double vertex[3][3];
	double normal[3][3];
	int foreColor[3];
	int backColor[3];
	float color[3];
	Triangle(int fore[3],int back[3])
	    {
        for(int i = 0; i < 3; i++){
			foreColor[i] = fore[i];
			color[i]=1.0*foreColor[i]/256;
        }
		for(int i = 0; i < 3; i++){
			backColor[i] = back[i];
        }
        } 
	Triangle(int in[6]){
		for(int i = 0; i < 3; i++){
			foreColor[i] = in[i];
			color[i]=1.0*foreColor[i]/256;
		}
		for(int i = 3; i < 6; i++){
			backColor[i - 3] = in[i];
		}
	}
	void loadVertex(int index, double in[6]){
		for(int i = 0; i < 3; i++){
			vertex[index][i] = in[i];
		}
		for(int i = 3; i < 6; i++){
			normal[index][i - 3] = in[i];
		}
	}
	void loadVertex(int index, double v1,double v2,double v3,double n1,double n2,double n3){
		vertex[index][0]=v1;
		vertex[index][1]=v2;
		vertex[index][3]=v3;
		normal[index][0]=n1;
		normal[index][1]=n2;
		normal[index][2]=n3;
	}
	void swap(Triangle& t){
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 3; j++){
				double tmp = this->vertex[i][j];
				this->vertex[i][j] = t.vertex[i][j];
				t.vertex[i][j] = tmp;
				tmp = this->normal[i][j];
				this->normal[i][j] = t.normal[i][j];
				t.normal[i][j] = tmp;
			}
			int tmp = this->foreColor[i];
			this->foreColor[i] = t.foreColor[i];
			t.foreColor[i] = tmp;
			tmp = this->backColor[i];
			this->backColor[i] = t.backColor[i];
			t.backColor[i] = tmp;
		}
	}
};
class TRIModel{
	

public:
	vector<Triangle> triangleList;
	double center[3];
	double Max[3];
	double Min[3];

	bool loadFromFile(const char* fileName);

	void copy(TRIModel *);
	TRIModel();
	~TRIModel();

};
