#pragma once

class Frustum;
class QuadTree {
public:
	QuadTree();
	~QuadTree();


	void Initialize(UINT mapWidth, UINT mapHeight);
	void CreateTree();

	//frustum ���ο� �ִ� ���ؽ� �޾ƿ�
	void getGrassPos(vector<D3DXVECTOR2>& vertexData, Frustum* frustum);
	//void setIndexByLOD(vector<UINT>& indexData, Frustum* frustum);
	
private:
	D3DXVECTOR3 cameraPos;
	UINT mapWidth, mapHeight;

	struct Node {

		Node* subNodes[4];
		Node *left, *down;
		UINT startX, startY;
		UINT width, height;

		D3DXVECTOR3 center;
		float radius;

		QuadTree* tree;

	//	UINT* indices;
	//	int LOD;

		
		Node() {
			subNodes[0] = subNodes[1] = subNodes[2] = subNodes[3] = nullptr;
			left = down = nullptr;
			startX = startY = 0;
			width = height = 0;
			//indices = nullptr;
		}
		~Node() {
			//delete[] indices;
		}
		//void getIndecies(UINT* & out_indices, UINT & out_width, UINT& out_height, UINT LOD);
		//void SetIndices(UINT startX_Idx, UINT startY_Idx, UINT mapWidth);


	};


	Node * head;


	void SetNode(Node* node, UINT startX_Idx, UINT startY_Idx, UINT width, UINT height);
	void DeleteTree(Node* node);

	
	
	void scanGrassPos(vector<D3DXVECTOR2>& out_vertexData, Node* node, Frustum* frustum);



	//quadtree�� lod ���ϰ� �Ǹ鼭 ��� ���� ����� �ٲ�
//	void Scan(vector<UINT>& out_indexData,Node* node, Frustum* frustum);
//	void SetNodeOLD(Node* node, UINT startX_Idx, UINT startY_Idx, UINT width, UINT height);

	//�̿� ��� üũ�� ���� �ڷᱸ��
	//������ ���ڴ�
	//map<Node*,UINT > nodeIdx;
	//map<UINT, Node*> nodeIdx_right;
	//map<UINT, Node*> nodeIdx_up;

	//void ClearLOD(Node * node);
	//void SetNeighbor(Node* node);
	
};