#include "BulletTower.h"
#include <stdlib.h>
#include <algorithm>

BulletTower::BulletTower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, std::vector<std::shared_ptr<Agent>>* agents, Mesh* bullet_mesh, Texture* bullet_texture, AudioSystem* audio) : Tower(mesh, shader, texture, position, agents)
{
	m_minDamage = 5.0f;
	m_maxDamage = 10.0f;
	m_damageRate = 3.0f;
	m_cooldown = 1.0f;
	m_accuracy = 0.7f;
	m_missAngle = 0.18f;
	m_rotX = (float)ToRadians(90.0f);
	m_radius = 15.0f;
	m_bMesh = bullet_mesh;
	m_bTexture = bullet_texture;
	m_audio = audio;
}

BulletTower::~BulletTower()
{
}

void BulletTower::Update(float timestep)
{
	if (m_target.expired()) // Weak pointer is expired = not currently locked on to an Agent
	{
		if (m_Agents->size() > 0) {
			std::shared_ptr<Agent> nearest = *(std::min_element(m_Agents->begin(), m_Agents->end(),
				[this](const std::shared_ptr<Agent> a, std::shared_ptr<Agent> b)
			{
				return Vector3::DistanceSquared(a->GetPosition(), m_position) < Vector3::DistanceSquared(b->GetPosition(), m_position); // Find the nearest Agent
			}));

			if (Vector3::DistanceSquared(nearest->GetPosition(), m_position) <= m_radius * m_radius)  // Check if this agent is within the targeting radius
			{
				m_target = nearest; // If it is, lock on to it
			}
		}
	}
	else
	{
		auto target = m_target.lock();
		float heading = atan2(target->GetPosition().x - m_position.x, target->GetPosition().z - m_position.z); // Heading of target position
		m_rotY = heading; // Rotate the tower to face the Agent

		// Spawn new bullets at the specified rate of fire
		m_cooldown -= m_damageRate * timestep;
		if (m_cooldown < 0) {
			m_cooldown = 1.0f; // Reset cooldown
			bool miss = ((float)rand()) / ((float)RAND_MAX) > m_accuracy; // Boolean for whether it's a missed bullet
			if (miss) { // Bullet will miss
				m_bullets.push_back(new Bullet(m_bMesh, m_shader, m_bTexture, m_position, \
					((float)rand()) / ((float)RAND_MAX) > 0.5? heading - m_missAngle: heading + m_missAngle)); // Ternary operator deciding whether tower under- or over-shoots (in terms of angle)
			}
			else {
				m_bullets.push_back(new Bullet(m_bMesh, m_shader, m_bTexture, m_position, heading)); // Spawn an accurate bullet
			}
			m_gunshotSound = m_audio->Play("Assets/Sounds/gunshot.wav", false); //Sound effect
			if (m_gunshotSound) {
				m_gunshotSound->SetVolume(0.1f);
				delete m_gunshotSound;
			}
		}
	}

	for (int i = m_bullets.size() - 1; i >= 0; --i) { // Iterate backwards allowing us to erase vector elements properly
		m_bullets[i]->Update(timestep);
		// Delete bullets that are out of the field of view to save memory
		if (abs(m_bullets[i]->GetPosition().x) > 40.0f || abs(m_bullets[i]->GetPosition().z) > 40.0f) {
			delete m_bullets[i];
			m_bullets.erase(m_bullets.begin() + i);
		}
		else {
			// Check if they collide with the Agent
			for (unsigned int j = 0; j < m_Agents->size(); j++) {
				CBoundingBox bb1 = (*m_Agents)[j]->GetBounds();
				CBoundingBox bb2 = m_bullets[i]->GetBounds();
				if (bb1.GetMax().x > bb2.GetMin().x &&
					bb1.GetMax().y > bb2.GetMin().y &&
					bb1.GetMax().z > bb2.GetMin().z &&
					bb1.GetMin().x < bb2.GetMax().x &&
					bb1.GetMin().y < bb2.GetMax().y &&
					bb1.GetMin().z < bb2.GetMax().z)
				{
					float damage = ((float)rand()) / (float)RAND_MAX * (m_maxDamage - m_minDamage) + m_minDamage; // If they do, deal damage between min and max damage
					(*m_Agents)[j]->Damage(damage);
					delete m_bullets[i]; // Delete the bullet
					m_bullets.erase(m_bullets.begin() + i);
					break;
				}
			}
		}
	}
}


void BulletTower::Render(Direct3D* renderer, Camera* cam) {
	GameObject::Render(renderer, cam);
	for (unsigned int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i]->Render(renderer, cam); // Render bullets
	}
}

void BulletTower::Upgrade()
{
	if (m_accuracy + 0.05 <= 1.0) {
		m_accuracy + 0.05; // Increase accuracy
	}
	m_damageRate += 1.0f; // Increase dmg rate
	m_minDamage += 0.1f; // Increase min dmg
	m_maxDamage += 0.1f; // Increase max dmg
	m_radius += 1.0f; // Increase radius
	SetUniformScale(m_scaleX+0.5f); // Make the tower slightly larger
}