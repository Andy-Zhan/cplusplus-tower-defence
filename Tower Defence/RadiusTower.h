#pragma once
#include "Tower.h"
#include "Agent.h"
#include <vector>
#include "StaticObject.h"
class RadiusTower :
	public Tower
{
private:
	StaticObject* m_circle; // Circle shaped object to represent area of effect
	Mesh* m_circleMesh; // Mesh for circle object
public:
	RadiusTower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, std::vector<std::shared_ptr<Agent>>* agents, Mesh* circleMesh);
	~RadiusTower();
	void Update(float timestep);
	void Render(Direct3D* renderer, Camera* cam);
	void Upgrade(); // Upgrades the tower
};

