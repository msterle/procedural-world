#ifndef MATERIAL_H
#define MATERIAL_H

#include "../include/glm/gtc/type_ptr.hpp"

struct Material {
	glm::vec4 ambient, diffuse, specular;
	float shininess;
};

namespace Materials {
	extern Material brass, bronze, polishedBronze, chrome, copper, polishedCopper, gold, polishedGold, pewter, silver, polishedSilver, emerald, jade, obsidian, pearl, ruby, turquoise, blackPlastic, blackRubber, bark;
}

#endif