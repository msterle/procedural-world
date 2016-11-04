#ifndef TREE_H
#define TREE_H

#include <vector>

#include "MeshInstance.h"

using namespace std;


class Tree {
public:
	Tree();
private:
	vector<MeshInstance> segments;
};

#endif