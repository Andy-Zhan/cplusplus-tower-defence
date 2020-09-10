/*	FIT2096 - Example Code
*	Game.cpp
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*/

#include "Game.h"
#include "TexturedShader.h"
#include "StaticObject.h"
#include "RadiusTower.h"
#include "BulletTower.h"
#include "DirectXTK/CommonStates.h"
#include <sstream>
#include "MathsHelper.h"

Game::Game()
{
	m_renderer = NULL;
	m_currentCam = NULL;
	m_input = NULL;
	m_meshManager = NULL;
	m_textureManager = NULL;
	m_stateMachine = NULL;
	m_agentManager = NULL;
	m_diffuseTexturedShader = NULL;
	m_unlitVertexColouredShader = NULL;
	m_spriteBatch = NULL;
	m_arialFont12 = NULL;
	m_arialFont18 = NULL;
	Button* m_bTowerButton = NULL;
	Button* m_rTowerButton = NULL;
	Button* m_upgradeButton = NULL;
	Button* m_nukeButton = NULL;

	// Adjust cost of purchases here
	m_bCost = 2000;
	m_rCost = 1500;
	m_uCost = 500;
	m_nCost = 10000;
}

Game::~Game() {}

bool Game::Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input)
{
	m_renderer = renderer;	
	m_audio = audio;
	m_input = input;
	m_meshManager = new MeshManager();
	m_textureManager = new TextureManager();
	m_highScore = 0;

	if (!InitShaders())
		return false;

	if (!LoadMeshes())
		return false;

	if (!LoadTextures())
		return false;

	if (!LoadAudio())
		return false;


	m_currentCam = new Camera();

	LoadFonts();
	InitGameWorld();
	InitUI();
	InitStates();

	return true;
}

bool Game::InitShaders()
{
	m_unlitVertexColouredShader = new Shader();
	if (!m_unlitVertexColouredShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/VertexColourPixelShader.ps"))
		return false;

	m_unlitScreenSpaceColouredShader = new Shader();
	if (!m_unlitScreenSpaceColouredShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/ScreenSpaceColourPixelShader.ps"))
		return false;
	
	m_diffuseTexturedShader = new TexturedShader();
	if (!m_diffuseTexturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/TexturedPixelShader.ps"))
		return false;

	return true;
}

bool Game::LoadMeshes()
{
	// Mesh for Agents
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/enemy.obj"))
		return false;

	// Mesh for Bullets
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/bullet.obj"))
		return false;

	// Mesh for Ground
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ground.obj"))
		return false;

	// Mesh for Tower
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/player_capsule.obj"))
		return false;

	// Mesh for Health Bar
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/progress_cube.obj"))
		return false;

	// Mesh for RadiusTower effect
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/disc.obj"))
		return false;

	return true;
}

void Game::LoadFonts()
{
	// There's a few different size fonts in there, you know
	m_arialFont12 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-12pt.spritefont");
	m_arialFont18 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-18pt.spritefont");
}


bool Game::LoadTextures()
{
	// Loading textures

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/ground.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redDarker.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redOrange.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_blue.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_green.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_purple.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/bullet.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/button.png"))
		return false;


	return true;
}

bool Game::LoadAudio()
{
	if (!m_audio->Load("Assets/Sounds/gunshot.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/explosion.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/spawn1.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/spawn2.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/nuke.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/gameover.wav"))
		return false;

	if (!m_audio->Load("Assets/Sounds/attack.wav"))
		return false;

	return true;
}

void Game::InitUI()
{
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());

	// Init button texture
	Texture* buttonTexture = m_textureManager->GetTexture("Assets/Textures/button.png");

	// Also init any buttons here
	std::wstringstream bCost;
	bCost << "+ Bullet Tower\n      $" << m_bCost;
	m_bCostText = bCost.str();

	std::wstringstream rCost;
	rCost << "+ Radius Tower\n       $" << m_rCost;
	m_rCostText = rCost.str();

	std::wstringstream uCost;
	uCost << "Upgrade Tower\n        $" << m_uCost;
	m_uCostText = uCost.str();

	std::wstringstream nCost;
	nCost << "Nuke 'em!\n  $" << m_nCost;
	m_nCostText = nCost.str();

	m_bTowerButton = new Button(128, 64, buttonTexture, m_bCostText.c_str(), Vector2(1200, 40), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Adds a new BulletTower
		BulletTower* bTower = new BulletTower(
			m_meshManager->GetMesh("Assets/Meshes/player_capsule.obj"),
			m_diffuseTexturedShader,
			m_textureManager->GetTexture("Assets/Textures/gradient_purple.png"),
			Vector3(MathsHelper::RandomRange(-25, 25), 0, MathsHelper::RandomRange(-15, 15)),
			m_agentManager->GetAgents(),
			m_meshManager->GetMesh("Assets/Meshes/bullet.obj"),
			m_textureManager->GetTexture("Assets/Textures/bullet.png"),
			m_audio);
		m_gameObjects.push_back(bTower);
		m_towers.push_back(bTower);
		m_agentManager->SpendFunds(m_bCost); // Decrease funds by cost
	});

	m_rTowerButton = new Button(128, 64, buttonTexture, m_rCostText.c_str(), Vector2(1200, 120), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Adds a new Radius Tower
		RadiusTower* rTower = new RadiusTower(
			m_meshManager->GetMesh("Assets/Meshes/player_capsule.obj"),
			m_diffuseTexturedShader,
			m_textureManager->GetTexture("Assets/Textures/gradient_blue.png"),
			Vector3(MathsHelper::RandomRange(-25, 25), 0, MathsHelper::RandomRange(-15, 15)),
			m_agentManager->GetAgents(),
			m_meshManager->GetMesh("Assets/Meshes/disc.obj"));
		m_gameObjects.push_back(rTower);
		m_towers.push_back(rTower);
		m_agentManager->SpendFunds(m_rCost); // Decrease funds by cost
	});

	m_nukeButton = new Button(128, 64, buttonTexture, m_nCostText.c_str(), Vector2(1200, 280), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		m_agentManager->Nuke(); // Call Nuke() method of agentManager class
		m_nukeSound = m_audio->Play("Assets/Sounds/nuke.wav", false); // Play nuke audio effect
		delete m_nukeSound;
		m_agentManager->SpendFunds(m_nCost);
	});

	m_upgradeButton = new Button(128, 64, buttonTexture, m_uCostText.c_str(), Vector2(1200, 200), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		m_towers[rand() % m_towers.size()]->Upgrade(); // Upgrade a random tower
		m_agentManager->SpendFunds(m_uCost);
	});
}

void Game::InitStates()
{
	// Our state machine needs to know its owner (so it only runs the callbacks while its owner exists)
	m_stateMachine = new StateMachine<GameStates, Game>(this, GameStates::START_STATE);

	// Let's match some states with with their OnEnter, OnUpdate, OnRender, OnExit callbacks.
	// Have a look in StateMachine.h to see how this works internally.

	m_stateMachine->RegisterState(GameStates::START_STATE, &Game::Start_OnEnter, // Game Start state
		&Game::Start_OnUpdate, &Game::Start_OnRender, &Game::Start_OnExit);

	m_stateMachine->RegisterState(GameStates::GAMEPLAY_STATE, &Game::Gameplay_OnEnter, // Gameplay state
		&Game::Gameplay_OnUpdate, &Game::Gameplay_OnRender, &Game::Gameplay_OnExit);

	m_stateMachine->RegisterState(GameStates::PAUSE_STATE, &Game::Pause_OnEnter, // When paused
		&Game::Pause_OnUpdate, &Game::Pause_OnRender, &Game::Pause_OnExit);

	m_stateMachine->RegisterState(GameStates::GAMEOVER_STATE, &Game::GameOver_OnEnter, // Game Over state
		&Game::GameOver_OnUpdate, &Game::GameOver_OnRender, &Game::GameOver_OnExit);
}

void Game::InitGameWorld()
{

	// Clear m_gameObjects, m_towers, and delete m_agentManager
	// This is relevant as this function is called when the game is reset
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		delete m_gameObjects[i];
	}
	m_gameObjects.clear();

	if (m_agentManager) {
		delete m_agentManager;
		m_agentManager = NULL;
	}
	m_towers.clear();
	
	// AgentManager object 
	m_agentManager = new AgentManager(
		m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
		m_diffuseTexturedShader,
		m_textureManager->GetTexture("Assets/Textures/bullet.png"), // Texture for small Agents (chose bullet.png for the sake of contrast with the red Agents)
		m_textureManager->GetTexture("Assets/Textures/gradient_redOrange.png"), // Texture for large Agents
		m_meshManager->GetMesh("Assets/Meshes/progress_cube.obj"), // Mesh for health bar
		m_textureManager->GetTexture("Assets/Textures/gradient_green.png"), // Texture for health bar
		m_audio);
	

	// Radius Tower positions
	Vector3 radiusTowerPos[R_TOWER_COUNT] = {
	Vector3(-15.0f,0.0f,-7.0f),
	Vector3(-5.0f, 0.0f,2.0f),
	Vector3(8.0f, 0.0f, -5.0f),
	Vector3(7.0f,0.0f,15.0f),
	Vector3(18.0f,0.0f,10.0f),
	};

	// Bullet Tower positions
	Vector3 bulletTowerPos[B_TOWER_COUNT] = {
	Vector3(-20.0f,0.0f,-4.0f),
	Vector3(-15.0f,0.0f,13.0f),
	Vector3(-5.0f, 0.0f,-10.0f),
	Vector3(2.0f, 0.0f, 8.0f),
	Vector3(14.0f,0.0f,4.0f),
	Vector3(20.0f,0.0f,-7.0f),
	};

	// Add a Radius tower at the positions
	for (unsigned int i = 0; i < R_TOWER_COUNT; i++) {
		RadiusTower* rTower = new RadiusTower(
			m_meshManager->GetMesh("Assets/Meshes/player_capsule.obj"),
			m_diffuseTexturedShader,
			m_textureManager->GetTexture("Assets/Textures/gradient_blue.png"),
			radiusTowerPos[i],
			m_agentManager->GetAgents(),
			m_meshManager->GetMesh("Assets/Meshes/disc.obj"));
		m_gameObjects.push_back(rTower);
		m_towers.push_back(rTower);
	}

	// Add a Bullet tower at the positions
	for (unsigned int i = 0; i < B_TOWER_COUNT; i++) {
		BulletTower* bTower = new BulletTower(
			m_meshManager->GetMesh("Assets/Meshes/player_capsule.obj"),
			m_diffuseTexturedShader,
			m_textureManager->GetTexture("Assets/Textures/gradient_purple.png"),
			bulletTowerPos[i],
			m_agentManager->GetAgents(),
			m_meshManager->GetMesh("Assets/Meshes/bullet.obj"),
			m_textureManager->GetTexture("Assets/Textures/bullet.png"),
			m_audio);
		m_gameObjects.push_back(bTower);
		m_towers.push_back(bTower);
	}

	// Add backgrounds to m_gameObjects
	StaticObject* spawn = new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_unlitScreenSpaceColouredShader, Vector3(-40.0f,0.02f,0.0f));
	spawn->SetXScale(0.1f);
	spawn->SetZScale(10.0f);
	m_gameObjects.push_back(spawn);
	StaticObject* ground = new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_diffuseTexturedShader, m_textureManager->GetTexture("Assets/Textures/ground.png"), Vector3(0.0f, 0.0f, 0.0f));
	ground->SetUniformScale(10.0f);
	m_gameObjects.push_back(ground);
	StaticObject* base = new StaticObject(m_meshManager->GetMesh("Assets/Meshes/ground.obj"), m_unlitScreenSpaceColouredShader, Vector3(40.0f, 0.02f, 0.0f));
	base->SetXScale(0.1f);
	base->SetZScale(10.0f);
	m_gameObjects.push_back(base);


}

void Game::Update(float timestep)
{
	m_input->BeginUpdate();

	// Assuming audio will be needed across multiple states
	m_audio->Update();

	m_stateMachine->Update(timestep);

	m_input->EndUpdate();
}

void Game::Render()
{
	m_renderer->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	m_stateMachine->Render();

	m_renderer->EndScene();		
}

void Game::Start_OnEnter()
{
}

void Game::Start_OnUpdate(float timestep)
{
	// Buttons need to update so they can check if the mouse is over them (they swap to a hover section of the image)
	if (m_input->GetKeyDown('S'))
	{
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}

	m_currentCam->Update(timestep);

}

void Game::Start_OnRender()
{
	
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
		m_gameObjects[i]->Render(m_renderer, m_currentCam); // Render all gameObjects
	DrawStartUI();
}

void Game::Start_OnExit()
{
}


void Game::Gameplay_OnEnter()
{
}

void Game::Gameplay_OnUpdate(float timestep)
{
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
		m_gameObjects[i]->Update(timestep); // Update gameObjects

	if (m_agentManager) {
		m_agentManager->Update(timestep); // Update agentManager
		if (m_agentManager->GetLives() < 1) { // Game over; no lives left
			m_gameoverSound = m_audio->Play("Assets/Sounds/gameover.wav", false); // Play sad trombone :(
			delete m_gameoverSound;
			m_stateMachine->ChangeState(GameStates::GAMEOVER_STATE);
		}
		if (m_agentManager->GetScore() > m_highScore) {
			m_highScore = m_agentManager->GetScore(); // Update highscore if the current score overtakes it
		}

	}

	if (*m_agentManager->GetFunds() < m_bCost) {
		m_bTowerButton->Disable(); // Disable button if not enough funds
	}
	else
	{
		m_bTowerButton->Enable();
		m_bTowerButton->Update(); // Else update 
	}
	
	if (*m_agentManager->GetFunds() < m_rCost) {
		m_rTowerButton->Disable();
	}
	else
	{
		m_rTowerButton->Enable();
		m_rTowerButton->Update();
	}

	if (*m_agentManager->GetFunds() < m_nCost) {
		m_nukeButton->Disable();
	}
	else
	{
		m_nukeButton->Enable();
		m_nukeButton->Update();
	}

	if (*m_agentManager->GetFunds() < m_uCost) {
		m_upgradeButton->Disable();
	}
	else
	{
		m_upgradeButton->Enable();
		m_upgradeButton->Update();
	}


	m_currentCam->Update(timestep); 

	// Pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::PAUSE_STATE);
	}

	// Reset game 
	if (m_input->GetKeyDown('R'))
	{

		InitGameWorld();

		m_stateMachine->ChangeState(GameStates::START_STATE);
	}
}

void Game::Gameplay_OnRender()
{

	// Render GameObjects
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
		m_gameObjects[i]->Render(m_renderer, m_currentCam);

	// Allow the Agents managed by AgentManager to be rendered
	m_agentManager->Render(m_renderer, m_currentCam);

	DrawGameUI();

	
}

void Game::Gameplay_OnExit()
{
	OutputDebugString("GamePlay OnExit\n");
}

void Game::Pause_OnEnter()
{
	OutputDebugString("Pause OnEnter\n");
}

void Game::Pause_OnUpdate(float timestep)
{
	// Check if we should exit pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}

	// Reset game (It would be annoying to have to unpause to reset)
	if (m_input->GetKeyDown('R'))
	{
		InitGameWorld(); //Re-initialise game world

		m_stateMachine->ChangeState(GameStates::START_STATE); // Reset to start state
	}
}

void Game::Pause_OnRender()
{
	// Keep drawing the game when paused (it's not being updated though which is what freezes it)
	Gameplay_OnRender();

	// In addition to the game, draw some UI to say we're paused
	DrawPauseUI();
}

void Game::Pause_OnExit()
{
	OutputDebugString("Pause OnExit\n");
}


void Game::GameOver_OnEnter()
{
	OutputDebugString("GameOver OnEnter\n");
}

void Game::GameOver_OnUpdate(float timestep)
{
	// Check if we should restart
	if (m_input->GetKeyDown('S'))
	{
		InitGameWorld();
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}
}

void Game::GameOver_OnRender()
{
	// Keep drawing the game when paused (it's not being updated though which is what freezes it)
	Gameplay_OnRender();

	// In addition to the game, draw some UI to say we're paused
	DrawGameOverUI();
}

void Game::GameOver_OnExit()
{
	OutputDebugString("GameOver OnExit\n");
}

void Game::DrawStartUI()
{
	BeginUI();

	m_arialFont18->DrawString(m_spriteBatch, L"Press S to Start", Vector2(545, 325), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));  // Draw text for start screen

	EndUI();
}

void Game::DrawGameUI()
{
	BeginUI();

	if (m_agentManager) {

		// Draw stats on left
		std::wstringstream hs;
		hs << "High Score: " << m_highScore;
		m_highScoreText = hs.str();

		std::wstringstream ss;
		ss << "Score: " << m_agentManager->GetScore();
		m_scoreText = ss.str();

		std::wstringstream ds;
		ds << "Enemies Destroyed: " << m_agentManager->GetKills();
		m_killCountText = ds.str();

		std::wstringstream fs;
		fs << "Funds: $" << *m_agentManager->GetFunds();
		m_arialFont18->DrawString(m_spriteBatch, fs.str().c_str(), Vector2(20, 130), Color(1.0f, 0.0f, 1.0f), 0, Vector2(0, 0));

		std::wstringstream ls;
		ls << "Lives: " << m_agentManager->GetLives();
		m_livesLeftText = ls.str();

		if (m_agentManager->isNewLevel()) {
			std::wstringstream nls;
			nls << "LEVEL " << m_agentManager->GetLevel();
			m_arialFont18->DrawString(m_spriteBatch, nls.str().c_str(), Vector2(585, 325), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
		}

		// High score
		m_arialFont18->DrawString(m_spriteBatch, m_highScoreText.c_str(), Vector2(20, 40), Color(1.0f, 1.0f, 1.0f), 0, Vector2(0, 0));
		// Current score
		m_arialFont18->DrawString(m_spriteBatch, m_scoreText.c_str(), Vector2(20, 70), Color(1.0f, 1.0f, 1.0f), 0, Vector2(0, 0));
		// Enemies destroyed
		m_arialFont18->DrawString(m_spriteBatch, m_killCountText.c_str(), Vector2(20, 100), Color(1.0f, 1.0f, 1.0f), 0, Vector2(0, 0));
		// Lives left: ternary operator makes the text red if only one life left
		m_arialFont18->DrawString(m_spriteBatch, m_livesLeftText.c_str(), Vector2(20, 160), m_agentManager->GetLives() > 1? Color(0.0f, 1.0f, 0.0f): Color(1.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	
		m_bTowerButton->Render();
		m_rTowerButton->Render();
		m_upgradeButton->Render();
		m_nukeButton->Render();
	}
	EndUI();
}

void Game::DrawPauseUI()
{
	BeginUI();
	// Paused text
	m_arialFont18->DrawString(m_spriteBatch, L"Paused", Vector2(605, 10), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	EndUI();
}

void Game::DrawGameOverUI()
{
	BeginUI();
	// Game Over text
	m_arialFont18->DrawString(m_spriteBatch, L"GAME OVER", Vector2(545, 285), Color(1.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	m_arialFont18->DrawString(m_spriteBatch, L"Press S to restart", Vector2(522, 315), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	EndUI();
}

void Game::BeginUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
}

void Game::EndUI()
{
	m_spriteBatch->End();
}


void Game::Shutdown()
{
	// Avoid memory leaks
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		delete m_gameObjects[i];
	}
	m_gameObjects.clear();

	m_towers.clear();

	if (m_agentManager)
	{
		delete m_agentManager;
		m_agentManager = NULL;
	}

	if (m_currentCam)
	{
		delete m_currentCam;
		m_currentCam = NULL;
	}

	if (m_unlitVertexColouredShader)
	{
		m_unlitVertexColouredShader->Release();
		delete m_unlitVertexColouredShader;
		m_unlitVertexColouredShader = NULL;
	}

	if (m_diffuseTexturedShader)
	{
		m_diffuseTexturedShader->Release();
		delete m_diffuseTexturedShader;
		m_diffuseTexturedShader = NULL;
	}

	if (m_meshManager)
	{
		m_meshManager->Release();
		delete m_meshManager;
		m_meshManager = NULL;
	}

	if (m_textureManager)
	{
		m_textureManager->Release();
		delete m_textureManager;
		m_textureManager = NULL;
	}

	if (m_stateMachine)
	{
		delete m_stateMachine;
		m_stateMachine = NULL;
	}

	if (m_spriteBatch)
	{
		delete m_spriteBatch;
		m_spriteBatch = NULL;
	}

	if (m_arialFont12)
	{
		delete m_arialFont12;
		m_arialFont12 = NULL;
	}

	if (m_arialFont18)
	{
		delete m_arialFont18;
		m_arialFont18 = NULL;
	}

	if (m_audio)
	{
		m_audio->Shutdown();
		delete m_audio;
		m_audio = NULL;
	}
}