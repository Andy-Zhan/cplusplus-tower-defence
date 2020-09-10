#pragma once
#include "GameObject.h"
#include "StaticObject.h"
#include "MathsHelper.h"
#include "CBoundingBox.h"

class AgentManager;

class Agent : public GameObject
{
	friend class AgentManager;
private:
	Vector3 m_targetPosition; // Position to move towards
	float m_moveSpeed; // Movement speed
	float m_rotateSpeed; // Rotation speed
	int m_waypoint; // Last waypoint reached
	int m_path; // Waypoint path that it is on
	float m_hp; // Healt
	StaticObject* m_healthBar; // Health bar object that hovers above the Agent
	CBoundingBox m_boundingBox; // Bounding box for collisions
	
	void Update(float timestep);
	void Render(Direct3D* renderer, Camera* cam);

public:
	Agent(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, Vector3 targetposition, int path, StaticObject* health_bar);
	~Agent();
	CBoundingBox GetBounds() { return m_boundingBox; };
	void Damage(float damage); // Damage the agent
};
