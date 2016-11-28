#include "Seeder.h"
#include "Terrain.h"
#include "ParaTree.h"
#include "Model.h"
#include <list>
#include <cstdlib>

using namespace std;

list<Model*> Seeder::seed(int count) {
	vector<ParaTree::TreeParams> presets;
	//presets.push_back(ParaTree::Presets::d); // 2D
	presets.push_back(ParaTree::Presets::d2);
	//presets.push_back(ParaTree::Presets::e); // 2D
	presets.push_back(ParaTree::Presets::g);
	presets.push_back(ParaTree::Presets::h);
	//presets.push_back(ParaTree::Presets::i); // leaves too large


	list<Model*> models;
	list<glm::vec3> positions;
	srand(glfwGetTime() * 1.0e6);
	for(int i = 0; i < count; ++i) {
		glm::vec3 treePos;
		bool posFound = false;
		float width = terrain->getWidth(), length = terrain->getLength();
		while(!posFound) {
			treePos = glm::vec3(fmod((float)rand(), width) - width / 2, 0, fmod((float)rand(), length) - length / 2);
			glm::vec3 norm = terrain->getNormalAtXZWorld(treePos.x, treePos.z);
			// if horizontal, prob is high, drops to 0 at vertical
			float prob = pow(norm.y, 4);
			// if higher altitude prob is lower
			prob *= exp(-terrain->getYAtXZWorld(treePos.x, treePos.z) / 10);
			// prob low right next to trees, high a small distance away
			/*
			for(glm::vec3 p : positions)
				prob += max(0.0, prob + sqrt(pow(10.0f / pow(p.x, 3) * sin(2 * p.x), 2) 
					+ pow(10.0f / pow(p.x, 3) * sin(2 * p.x), 2)));
					*/
			float random = (float)rand() / RAND_MAX;
			if(prob > random)
				posFound = true;
		}
		treePos.y = terrain->getYAtXZWorld(treePos.x, treePos.z);
		ParaTree::TreeParams params = presets[rand() % presets.size()];
		params.n = params.n - round(((float)rand() / RAND_MAX) * 0.25 * params.n);
		ParaTree* ptree = new ParaTree(params, barkTex, leafTex);
		ptree->translate(treePos);
		models.push_back(ptree);
		positions.push_back(treePos);
	}
	return models;
}