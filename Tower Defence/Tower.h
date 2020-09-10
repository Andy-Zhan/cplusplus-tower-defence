#pragma once
#include "GameObject.h"
#include "Agent.h"
#include "InputController.h"
#include "Camera.h"
#include <vector>
#include <memory>
class Tower :
	public GameObject
{
protected:

	// All the common traits of the types of Towers
	std::vector<std::shared_ptr<Agent>>* m_Agents; // Store the pointer to the vector of shared ptrs to Agents
	float m_minDamage; // Min damage
	float m_maxDamage; // Max damage
	float m_damageRate; // Rate of fire/damage
	float m_cooldown; // Tracks cooldown time
	float m_radius; // Radius of damage (Radius tower), or radius for locking on to an Agent (Bullet tower)

public:

	Tower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, std::vector<std::shared_ptr<Agent>>* agents);
	virtual void Upgrade() {}; // virtual Upgrade method
};

