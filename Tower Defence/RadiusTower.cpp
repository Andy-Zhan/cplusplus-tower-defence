#include "RadiusTower.h"
#include <stdlib.h>
#include "MathsHelper.h"

RadiusTower::RadiusTower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, std::vector<std::shared_ptr<Agent>>* agents, Mesh* circleMesh) : Tower(mesh, shader, texture, position, agents)
{ 
	m_radius = 5.0f;
	m_minDamage = 15.0f;
	m_maxDamage = 25.0f;
	m_damageRate = 0.5;
	m_cooldown = 1.0f;
	m_circleMesh = circleMesh;
	m_circle = new StaticObject(circleMesh, shader, texture);
	m_circle->SetXScale(0.0f);
	m_circle->SetZScale(0.0f);
	m_circle->SetPosition(Vector3(position.x, position.y-m_circleMesh->GetMax().y+0.01, position.z)); // Centre the circle on the tower, but slightly above the ground so it is properly visible
}


RadiusTower::~RadiusTower()
{
	delete m_circle; // delete the circle object
}

void RadiusTower::Update(float timestep)
{
	m_cooldown -= m_damageRate*timestep;
	m_circle->SetXScale((1 / m_circleMesh->GetMin().x)*(m_cooldown)*m_radius); // Set the X scale based on the cooldown time
	m_circle->SetZScale((1 / m_circleMesh->GetMin().z)*(m_cooldown)*m_radius); // Set the Z scale based on the cooldown time
	if (m_cooldown < 0) { // Check if it is time to fire
		m_cooldown = 1.0f;
		for (unsigned int i = 0; i < m_Agents->size(); i++) {
			if (Vector3::DistanceSquared((*m_Agents)[i]->GetPosition(), m_position) <= m_radius*m_radius) {
				float damage = ((float)rand()) / (float)RAND_MAX * (m_maxDamage - m_minDamage) + m_minDamage; // If the Agent is within the radius, damage them
				(*m_Agents)[i]->Damage(damage);
			}
		}
	}
}

void RadiusTower::Render(Direct3D* renderer, Camera* cam)
{
	Tower::Render(renderer, cam);
	m_circle->Render(renderer, cam);

}

void RadiusTower::Upgrade()
{
	// Upgrade tower
	m_radius += 1.0f; // Increase radius
	m_damageRate += 0.2f; // Increase damage
	m_minDamage += 0.5f;
	m_maxDamage += 0.5f;
	SetUniformScale(m_scaleX+0.5f); // Make slightly larger
}