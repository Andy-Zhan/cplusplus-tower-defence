/*	FIT2096 - Example Code
*	Game.h
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	This class is the heart of our game and is also where our game logic will reside
*	It contains the overall Update and Render method for the whole game
*	You should eventually split some game logic into other objects which Game will manage
*	Watch the size of this class - it can grow out of control very fast!
*/

#ifndef GAME_H
#define GAME_H

#include "Direct3D.h"
#include "Camera.h"
#include "InputController.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "AudioSystem.h"
#include "AgentManager.h"
#include "StateMachine.h"
#include "GameObject.h"
#include "Button.h"
#include "Tower.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"


#include <vector>

class Game
{
private:

	// All the states our game can be in. The STATE_COUNT down the bottom is just an easy way
// for us to count how many states this enum has (cast this to an int)
	enum class GameStates
	{
		START_STATE,
		GAMEPLAY_STATE,
		PAUSE_STATE,
		GAMEOVER_STATE,
		STATE_COUNT
	};

	Camera* m_currentCam;		
	Direct3D* m_renderer;
	AudioSystem* m_audio;
	InputController* m_input;
	MeshManager* m_meshManager;
	TextureManager* m_textureManager;

	// Buttons
	Button* m_bTowerButton;
	Button* m_rTowerButton;
	Button* m_nukeButton;
	Button* m_upgradeButton;

	// Sfx
	AudioClip* m_nukeSound;
	AudioClip* m_gameoverSound;

	// Tower costs
	int m_bCost;
	int m_rCost;
	int m_uCost;
	int m_nCost;
	
	// Sprites / Fonts
	SpriteBatch* m_spriteBatch;
	SpriteFont* m_arialFont12;
	SpriteFont* m_arialFont18;

	// Stores high score
	int m_highScore;

	// Wide strings use more than 8 bits per character so they can capture more symbols
	// Windows loves them and as such so does Direct3D and its text helpers
	std::wstring m_scoreText;
	std::wstring m_highScoreText;
	std::wstring m_killCountText;
	std::wstring m_livesLeftText;
	std::wstring m_bCostText;
	std::wstring m_rCostText;
	std::wstring m_nCostText;
	std::wstring m_uCostText;

	Shader* m_unlitVertexColouredShader;
	Shader* m_unlitScreenSpaceColouredShader;
	Shader* m_diffuseTexturedShader;

	// StateMachine
	StateMachine<GameStates, Game>* m_stateMachine;
	
	AgentManager* m_agentManager;
	std::vector <GameObject*> m_gameObjects;
	std::vector <Tower*> m_towers;
	static const int R_TOWER_COUNT = 5; // Number of Radius Towers
	static const int B_TOWER_COUNT = 6; // Number of Bullet Towers

	// Splitting initialisation up into several steps
	bool InitShaders();
	bool LoadMeshes();
	void InitStates();
	bool LoadTextures();
	bool LoadAudio();
	void LoadFonts();
	void InitGameWorld();

	// To draw the UIs for different states
	void InitUI();
	void DrawStartUI();
	void DrawGameUI();
	void DrawPauseUI();
	void DrawGameOverUI();
	void BeginUI();
	void EndUI();

	// OnEnter, OnUpdate, OnRender, OnExit functions for each state

	void Start_OnEnter();
	void Start_OnUpdate(float timestep);
	void Start_OnRender();
	void Start_OnExit();

	void Gameplay_OnEnter();
	void Gameplay_OnUpdate(float timestep);
	void Gameplay_OnRender();
	void Gameplay_OnExit();

	void Pause_OnEnter();
	void Pause_OnUpdate(float timestep);
	void Pause_OnRender();
	void Pause_OnExit();

	void GameOver_OnEnter();
	void GameOver_OnUpdate(float timestep);
	void GameOver_OnRender();
	void GameOver_OnExit();

public:
	Game();	
	~Game();

	bool Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input); //The initialise method will load all of the content for the game (meshes, textures, etc.)

	void Update(float timestep);	//The overall Update method for the game. All gameplay logic will be done somewhere within this method
	void Render();					//The overall Render method for the game. Here all of the meshes that need to be drawn will be drawn

	void Shutdown(); //Cleanup everything we initialised
};

#endif