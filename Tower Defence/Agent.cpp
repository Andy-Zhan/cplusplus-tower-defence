#include "Agent.h"
#include <stdlib.h>
#include <cmath>


Agent::Agent(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, Vector3 targetposition, int path, StaticObject* health_bar): GameObject::GameObject(mesh, shader, texture, position)
{
	m_targetPosition = targetposition;
	m_rotY = acos((m_targetPosition - m_position).z / (m_targetPosition - m_position).Length()); // Face next waypoint
	m_waypoint = 0; // Current waypoint = the first one in the path
	m_rotateSpeed = 0.005f;
	m_path = path;
	
	// 'Owns' a health bar object
	m_healthBar = health_bar;
	m_healthBar->SetUniformScale(3.0f);

	// Initialise bounding box
	m_boundingBox = CBoundingBox(
		m_position + m_mesh->GetMin(),
		m_position + m_mesh->GetMax());
	
}

Agent::~Agent() {
	delete m_healthBar; // delete the health bar
}

void Agent::Update(float timestep) 
{

	float heading = atan2(m_targetPosition.x - m_position.x, m_targetPosition.z - m_position.z); // Heading of target position
	if (heading < 0)
		heading = 2*PI + heading; // -180 to 180 becomes 0 to 360

	if (abs(heading - m_rotY) <= 0.1) {
		m_position = Vector3::Lerp(m_position, m_targetPosition, m_moveSpeed * timestep); // Move in the correct direction
	}
	else {
		// Direction to turn in
		float rotate_angle = heading - m_rotY;
		if (rotate_angle > PI)
			rotate_angle -= 2 * PI;
		if (rotate_angle < -PI)
			rotate_angle += 2 * PI;
		if (rotate_angle > 0)
			m_rotY += m_rotateSpeed;
		else
			m_rotY -= m_rotateSpeed;
	}

	// Update position
	m_boundingBox.SetMin(m_position +
		Vector3(m_mesh->GetMin().x*m_scaleX, m_mesh->GetMin().y*m_scaleY, m_mesh->GetMin().z*m_scaleZ));
	m_boundingBox.SetMax(m_position +
		Vector3(m_mesh->GetMax().x*m_scaleX, m_mesh->GetMax().y*m_scaleY, m_mesh->GetMax().z*m_scaleZ));

	// Set the length of the health bar depending on how much health remains
	m_healthBar->SetXScale(m_hp / 20.0f);
	m_healthBar->SetPosition(m_position + Vector3(0.0f, m_scaleY / 2 + 2.0f, 0.0f));
}

void Agent::Render(Direct3D* renderer, Camera* cam)
{
	GameObject::Render(renderer, cam);
	m_healthBar->Render(renderer, cam); // Also render the health bar

}

void Agent::Damage(float damage)
{
	m_hp -= damage; // Decrement health equal to damage taken
}