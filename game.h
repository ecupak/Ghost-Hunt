#pragma once

#include <vector>
#include <random> // Mersenne Twister,  uniform distribution, and random device

#include "template.h"
#include "seeker.h"
#include "ghost.h"


namespace Tmpl8 {

class Surface;
class Game
{
public:
	Game();
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();	
	void Tick( float deltaTime );
	void MouseUp(int button) {};
	void MouseDown(int button);
	void MouseMove(int x, int y);
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key ) { /* implement if you want to handle keys */ }

private:
	// METHODS
	void AddGhost(char* _path);
	void StartNewGame();
	void ShowStartScreen();
	void ResetGhosts();
	void RepositionGhosts();
	bool GetIsGhostOverlapping(Ghost& ghost);
	void DrawGhosts();
	void ShowFoundGhost(Ghost& ghost);
	void ShowScore();
	void CheckForGameOver(Ghost& ghost);
	void CheckIfGhostWasFound(Ghost& ghost);
	void GhostFound(Ghost& ghost);
	bool GetIsGameOver();
	void EndGame();	

	// ATTRIBUTES
	Surface* screen{ nullptr };
	Surface* clean_screen{ nullptr };
	Surface* ghost_screen{ nullptr };
	Seeker seeker;
	int mouseX{ 100 };
	int mouseY{ 100 };
	bool isWaitingToStart{ true };
	std::vector<Ghost> ghosts;
	std::vector<Ghost> foundGhosts;
	std::mt19937 twister{ std::random_device{}() };
	float ghost_detection_range{ 30.0f };
	int max_placement_attempts{ 300 };
};

}; // namespace Tmpl8