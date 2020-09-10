#include "Bullet.h"



Bullet::Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float direction): GameObject(mesh, shader, texture, position)
{
	SetUniformScale(7.0f);

	m_rotY = direction; // Face in the direction of travel
	// Initialise bounding box
	m_boundingBox = CBoundingBox(
		m_position + m_mesh->GetMin(),
		m_position + m_mesh->GetMax());
	m_moveSpeed = 100.0f; // Set bullet speed
}


Bullet::~Bullet()
{
}

void Bullet::Update(float timestep) {

	// We'll start by declaring a forward vector in world space
	Vector3 worldForward = Vector3(0, 0, 1);

	// Next we'll wrap up our Y rotation in a matrix (remember matrices transform vectors)
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	// Finally, we'll transform our world forward vector by the heading matrix which 
	// will essentially spin it from a world forward into a local forward which takes
	// the object's rotation into account.
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);
	m_position += localForward * m_moveSpeed * timestep;

	// Update bounding box
	m_boundingBox.SetMin(m_position +
		Vector3(m_mesh->GetMin().x*m_scaleX, m_mesh->GetMin().y*m_scaleY, m_mesh->GetMin().z*m_scaleZ));
	m_boundingBox.SetMax(m_position +
		Vector3(m_mesh->GetMax().x*m_scaleX, m_mesh->GetMax().y*m_scaleY, m_mesh->GetMax().z*m_scaleZ));
}