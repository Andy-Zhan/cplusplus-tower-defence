#include "Tower.h"


Tower::Tower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, std::vector<std::shared_ptr<Agent>>* agents): GameObject(mesh, shader, texture, position)
{
	m_Agents = agents;
	SetUniformScale(2.0f); // Towers start with an uniform scale of 2.0

}

