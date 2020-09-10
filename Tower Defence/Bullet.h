#pragma once
#include "GameObject.h"
#include "CBoundingBox.h"
class Bullet :
	public GameObject
{
private:
	CBoundingBox m_boundingBox; // Bounding box for collisions
	float m_moveSpeed; // Bullet speed
public:
	Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float direction);
	~Bullet();
	void Update(float timestep);
	CBoundingBox GetBounds() { return m_boundingBox; }
};

