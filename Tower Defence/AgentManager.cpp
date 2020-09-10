#include "AgentManager.h"
#include <stdlib.h>

AgentManager::AgentManager(Mesh* mesh, Shader* shader, Texture* texture1, Texture* texture2, Mesh* hp_mesh, Texture* hp_texture, AudioSystem* audio)
{
	m_spawnRate = 0.5f;
	m_spawnTimer = 0.9f;
	m_mesh = mesh;
	m_shader = shader;
	m_texture1 = texture1;
	m_texture2 = texture2;
	m_hpTexture = hp_texture;
	m_hpMesh = hp_mesh;
	m_agentSpeed = 4.0f;
	m_killCount = 0;
	m_newLevel = false;
	m_score = 0;
	m_lives = 4;
	m_level = 1;
	m_funds = 0;
	m_spawnCount = 0;
	m_amountToSpawn = 10;
	m_audio = audio;
	m_explosionSound = NULL;
	m_spawnSound1 = NULL;
	m_spawnSound2 = NULL;
	m_attackSound = NULL;
}


AgentManager::~AgentManager()
{
	m_vAgents.clear();
}

void AgentManager::Update(float timestep)
{
	if (m_spawnCount < m_amountToSpawn) { // Check if not enough Agents spawned yet
		m_spawnTimer += m_spawnRate * timestep; // Increment spawn timer

	// If timer reached, spawn Agent and reset timer
		if (m_spawnTimer >= 1) {
			spawnAgent();
			m_spawnCount += 1; // Increment spawn count
			m_spawnTimer = 0.0f;
		}
	}
	else {
		if (m_vAgents.size() == 0 && m_newLevel == false) { // If no more Agents remaining (all have been destroyed)
			m_newLevel = true; // Start a new level
			m_level += 1;
			m_funds += m_level * 250 > 2500? 2500: m_level*250; // Add funds (max $2500)
			m_countDown = 3.0f; // Short delay before start
		}
	}

	if (m_newLevel) {
		if (m_countDown <= 0) {
			m_newLevel = false;
			// Reset spawn counters, make Agents more difficult to destroy (+spawn rate, speed, amount)
			m_spawnCount = 0;
			m_spawnTimer = 0.9;
			m_amountToSpawn += 5;
			m_spawnRate += 0.2;
			m_agentSpeed += 0.2;
		}
		else {
			m_countDown -= 0.005;
		}
	}
	// Update Agents
	for (unsigned int i = 0; i < m_vAgents.size(); i++) {
		// No health left
		if (m_vAgents[i]->m_hp <= 0){
			m_score += 240; // Increment score
			m_killCount += 1;
			m_vAgents.erase(m_vAgents.begin() + i);
			// Play explosion effect
			m_explosionSound = m_audio->Play("Assets/Sounds/explosion.wav", false);
			m_explosionSound->SetVolume(0.4f);
			delete m_explosionSound;
			continue;
		}

		m_vAgents[i]->Update(timestep);

		if (Vector3::DistanceSquared(m_vAgents[i]->m_targetPosition, m_vAgents[i]->m_position) <= 2.0f) { // If waypoint reached by an agent:
			m_vAgents[i]->m_waypoint += 1; // Update its last waypoint reached
			if (m_vAgents[i]->m_waypoint == WAYPOINT_COUNT) { // If there are no more waypoints:
				m_attackSound = m_audio->Play("Assets/Sounds/attack.wav", false);
				delete m_attackSound;
				m_lives -= 1; // Remove a life
				m_vAgents.erase(m_vAgents.begin() + i);
			}
			else {
				m_vAgents[i]->m_targetPosition = m_waypoints[m_vAgents[i]->m_path][m_vAgents[i]->m_waypoint]; // Update its target location
			}

		}
	}
}

void AgentManager::Render(Direct3D* renderer, Camera* cam) {
	// Render Agent objects
	for (unsigned int i = 0; i < m_vAgents.size(); i++) {
		m_vAgents[i]->Render(renderer, cam);
	}
}

void AgentManager::spawnAgent() {

	float default_size = 2.0f; // Size of small Agents
	int path = rand() % PATH_COUNT; // Choose a random path
	if (rand() % 2 == 0) { // Coin flip: Generate one of the two Agent types
		m_vAgents.push_back(std::make_shared<Agent>(m_mesh, m_shader, m_texture1, m_waypoints[path][0], m_waypoints[path][1], path, new StaticObject(m_hpMesh, m_shader, m_hpTexture)));
		m_vAgents[m_vAgents.size() - 1]->SetUniformScale(default_size);
		m_spawnSound1 = m_audio->Play("Assets/Sounds/spawn1.wav", false);
		m_spawnSound1->SetVolume(0.2f);
		delete m_spawnSound1;
	}
	else {
		m_vAgents.push_back(std::make_shared<Agent>(m_mesh, m_shader, m_texture2, m_waypoints[path][0], m_waypoints[path][1], path, new StaticObject(m_hpMesh, m_shader, m_hpTexture)));
		m_vAgents[m_vAgents.size() - 1]->SetUniformScale(default_size*2);
		m_spawnSound2 = m_audio->Play("Assets/Sounds/spawn2.wav", false);
		m_spawnSound2->SetVolume(0.2f);
		delete m_spawnSound2;
	}
	m_vAgents[m_vAgents.size() - 1]->m_moveSpeed = m_agentSpeed / m_vAgents[m_vAgents.size() - 1]->GetXScale(); // Set the speed of Agents inversely proportional to their size
	m_vAgents[m_vAgents.size() - 1]->m_hp = m_vAgents[m_vAgents.size() - 1]->GetXScale()*25.0f; // Larger agents have more health
}

void AgentManager::Nuke() {
	for (unsigned int i = 0; i < m_vAgents.size(); i++) {
		m_vAgents[i]->Damage(m_vAgents[i]->m_hp); // Deal damage equal to the HP of each agent
	}
}
