#pragma once

#include "GameObject.h"
#include "StaticObject.h"
#include "AudioSystem.h"
#include "AudioClip.h"
#include "Agent.h"

#include <vector>
#include <memory>

class AgentManager
	/*
	Manages the Agents.
	I chose to create this AgentManager class for the following reasons:
		- It allows us to easily manage properties affecting all Agents such as spawnRate and spawnTimer,
		  without having to inappropriately make them members of the Game class (they only affect Agents after all)
		- Similarly, Agents that share the same path can be easily managed with regard to Waypoints, without having to
		  pass each Agent an entire array.
	*/
{
private:

	static const int PATH_COUNT = 4; // Number of paths for Agents
	static const int WAYPOINT_COUNT = 7; // Waypoints per path
	float m_spawnRate; // Spawn rate of Agents
	float m_spawnTimer; // Keeps track of time until next spawn
	float m_agentSpeed; // Controls speed of Agents
	std::vector<std::shared_ptr<Agent>> m_vAgents; // Vector of shared pointers to Agents managed by each AgentManager object
	Mesh* m_mesh; // Mesh (passed to Agent object)
	Mesh* m_hpMesh; // Mesh of health bars
	Shader* m_shader; // Shader (passed to Agent object)
	Texture* m_texture1; // Texture (small Agents) (passed to Agent object)
	Texture* m_texture2; // Texture (large Agents) (passed to Agent object)
	Texture* m_hpTexture; // Texture of health bars
	int m_killCount; // Number of enemies destroyed
	int m_lives; // Lives left in base
	int m_score; // Current score
	int m_level; // Current level
	int m_funds; // Current funds
	int m_spawnCount; // How many Agents have spawned in this level so far
	int m_amountToSpawn; // How many Agents to spawn in this level
	bool m_newLevel; // True if a new level is about to start, else False
	float m_countDown; // Time to wait before starting a new level
	AudioSystem* m_audio; // Pointer to the AudioSystem
	void spawnAgent(); // Spawns a new agent

	// Sound effects
	AudioClip* m_explosionSound;
	AudioClip* m_spawnSound1;
	AudioClip* m_spawnSound2;
	AudioClip* m_attackSound;

	// Waypoints for Agents
	Vector3 m_waypoints[PATH_COUNT][WAYPOINT_COUNT] = {
		{
		Vector3(-35.0f,0.0f,15.0f),
		Vector3(-23.0f, 0.0f,14.0f),
		Vector3(-12.0f, 0.0f, 13.0f),
		Vector3(0.0f, 0.0f, 15.0f),
		Vector3(7.0f,0.0f,14.0f),
		Vector3(15.0f,0.0f,16.0f),
		Vector3(29.0f,0.0f,13.0f)
		},
		{
		Vector3(-35.0f,0.0f,10.0f),
		Vector3(-23.0f, 0.0f,9.0f),
		Vector3(-12.0f, 0.0f, 8.0f),
		Vector3(0.0f, 0.0f, 6.0f),
		Vector3(7.0f,0.0f,7.0f),
		Vector3(15.0f,0.0f,8.0f),
		Vector3(29.0f,0.0f,9.0f)
		},
		{
		Vector3(-35.0f,0.0f,0.0f),
		Vector3(-21.0f, 0.0f,-5.0f),
		Vector3(-12.0f, 0.0f, -2.0f),
		Vector3(0.0f, 0.0f, -3.0f),
		Vector3(7.0f,0.0f,-4.0f),
		Vector3(13.0f,0.0f,-6.0f),
		Vector3(29.0f,0.0f,-1.0f)
		},
		{
		Vector3(-35.0f,0.0f,-10.0f),
		Vector3(-21.0f, 0.0f,-15.0f),
		Vector3(-12.0f, 0.0f, -8.0f),
		Vector3(0.0f, 0.0f, -11.0f),
		Vector3(7.0f,0.0f,-7.0f),
		Vector3(13.0f,0.0f,-12.0f),
		Vector3(29.0f,0.0f,-9.0f)
		}
	}; 
	
public:

	AgentManager(Mesh* mesh, Shader* shader, Texture* texture1, Texture* texture2, Mesh* hp_mesh, Texture* hp_texture, AudioSystem* audio); //Constructor
	~AgentManager(); // Destructor

	std::vector<std::shared_ptr<Agent>>* GetAgents() { return &m_vAgents; } // Returns a pointer to a vector of shared pointers to Agents
	int GetScore() { return m_score; }
	int GetKills() { return m_killCount; }
	int GetLives() { return m_lives; }
	int GetLevel() { return m_level; }
	int isNewLevel() { return m_newLevel; }
	int* GetFunds() { return &m_funds; }
	void SpendFunds(int cost) { m_funds -= cost; }
	void Nuke(); // Destroys all Agents currently existing
	
	void Update(float timestep);
	void Render(Direct3D* renderer, Camera* cam);

};

