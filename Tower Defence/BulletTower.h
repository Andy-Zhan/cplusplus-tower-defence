#pragma once
#include "Tower.h"
#include "Agent.h"
#include "Bullet.h"
#include <vector>
#include "AudioClip.h"
#include "AudioSystem.h"

class BulletTower :
	public Tower
{

private:

	float m_accuracy; // Accurcy of bullet tower
	float m_missAngle; //The angle offset (in radians) of a bullet from its target if it is supposed to miss
	std::weak_ptr<Agent> m_target; // Weak pointer to target enemy
	Mesh* m_bMesh; // Mesh for bullet
	Texture* m_bTexture; // Texture for bullet
	std::vector<Bullet*> m_bullets; // Vector containing bullets owned by tower
	AudioSystem* m_audio; // Pointer to the AudioSystem
	AudioClip* m_gunshotSound; // Gunshot sfx

public:

	BulletTower(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, std::vector<std::shared_ptr<Agent>>* agents, Mesh* bullet_mesh, Texture* bullet_texture, AudioSystem* audio);
	~BulletTower();
	void Update(float timestep);
	void Render(Direct3D* renderer, Camera* cam);

	void Upgrade(); // Upgrades the tower

};


