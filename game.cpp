#include <cstdio> //printf
#include <string>

#include "game.h"
#include "surface.h"


namespace Tmpl8
{
	// -----------------------------------------------------------
	// Constants
	// -----------------------------------------------------------
	const unsigned int RED{ 0xFF0000 };
	const unsigned int YELLOW{ 0xCCC000 };
	const unsigned int GREEN{ 0x00FF00 };
	const unsigned int BLACK{ 0x000000 };
	const unsigned int TRANSPARENT_BLACK{ 0xFF000000 };
	
	// -----------------------------------------------------------
	// Constructor
	// -----------------------------------------------------------
	Game::Game() :
		seeker{ Seeker()}
	{}

	// -----------------------------------------------------------
	// Mouse events
	// -----------------------------------------------------------
	void Game::MouseDown(int button)
	{
		if (isWaitingToStart)
		{
			isWaitingToStart = false;
			StartNewGame();
		}
	}


	void Game::MouseMove(int x, int y)
	{
		mouseX = x;
		mouseY = y;
	}

	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		// Load up background image.
		clean_screen = new Surface("assets/noise.png");

		// Load up ghost stuff.
		ghost_screen = new Surface(ScreenWidth, ScreenHeight);

		// Art obtained from https://bulbapedia.bulbagarden.net/ by Ken Sugimori for Pokemon.
		AddGhost("assets/gastly-s.png");
		AddGhost("assets/haunter-s.png");
		AddGhost("assets/gengar-s.png");

		// Share material and ghost info with seeker.
		seeker.SetSurface(screen);
		seeker.SetGhostInfo(ghost_screen, &ghosts);
	}


	// -----------------------------------------------------------
	// Add Ghosts
	// -----------------------------------------------------------
	void Game::AddGhost(char* _path)
	{
		Ghost ghost(new Sprite(new Surface(_path), 1));
		ghosts.push_back(ghost);
	}


	// -----------------------------------------------------------
	// Hide ghosts and place them randomly
	// -----------------------------------------------------------
	void Game::StartNewGame()
	{
		ResetGhosts();
		RepositionGhosts();
		DrawGhosts();
	}


	// Hide ghosts and make all active.
	void Game::ResetGhosts()
	{
		for (Ghost& ghost : ghosts)
			ghost.Reset();		
	}


	// -----------------------------------------------------------
	// Move ghosts around.
	// -----------------------------------------------------------
	void Game::RepositionGhosts()
	{	
		// Set boundaries.
		int left = 20, right = ghost_screen->GetWidth() - 20;
		int top = 20, bottom = ghost_screen->GetHeight() - 20;
		
		// Random x,y generators within bounds (using Mersenne Twister).
		// Credit: Alex (https://www.learncpp.com/cpp-tutorial/generating-random-numbers-using-mersenne-twister/)
		std::uniform_int_distribution<> randX{ left, right };
		std::uniform_int_distribution<> randY{ top, bottom };

		// Try to place.
		for (auto& ghost : ghosts)
		{
			bool out_of_bounds{ true };
			bool overlapping{ true };
			int placement_attempts{ max_placement_attempts };

			do
			{
				// Get new position.
				int x = randX(twister), y = randY(twister);
				ghost.UpdateBoundaries(vec2(x, y));

				// Check if ghost overlaps another ghost or is outside the screen.
				out_of_bounds = (ghost.left < left || ghost.right > right || ghost.top < top || ghost.bottom > bottom);
				overlapping = GetIsGhostOverlapping(ghost);

				placement_attempts--;
			} while ((out_of_bounds || overlapping) && placement_attempts > 0);

			// Exclude ghost from current game if couldn't be placed.
			if (placement_attempts <= 0)
				ghost.SetInactive();
			else
				ghost.isActive = true;
		}		
	}


	// Check that ghost doesn't overlap other ghosts.
	bool Game::GetIsGhostOverlapping(Ghost& ghost)
	{
		for (auto& other_ghost : ghosts)
		{
			// Skip if comparing same ghost or an inactive ghost.
			if (other_ghost.id == ghost.id || other_ghost.isActive == false) continue;

			// Credit: https://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other
			if (ghost.left < other_ghost.right && ghost.right > other_ghost.left && ghost.top < other_ghost.bottom && ghost.bottom > other_ghost.top)
				return true;			
		}
		return false;
	}


	// Draw the ghosts.
	void Game::DrawGhosts()
	{		
		ghost_screen->Clear(BLACK);
		for (Ghost& ghost : ghosts)
		{
			if (ghost.isFound || ghost.isActive == false) continue;

			ghost.sprite->Draw(ghost_screen, ghost.m_position.x, ghost.m_position.y);
		}
	}


	// -----------------------------------------------------------
	// When all ghosts are found, game over
	// -----------------------------------------------------------
	// Redraw ghost screen without found ghost(s)
	void Game::GhostFound(Ghost& ghost)
	{
		ghost.isFound = true;
		DrawGhosts();
	}


	// If all ghosts are revealed, start new game.
	bool Game::GetIsGameOver()
	{
		for (Ghost& ghost : ghosts)
		{			
			if (ghost.isRevealed == false && ghost.isActive)			
				return false;			
		}
		return true;
	}
	

	// Wait for player to start game (ghosts from prior game will be showing).
	void Game::EndGame()
	{
		isWaitingToStart = true;
	}


	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{
	}


	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------
	void Game::Tick(float deltaTime)
	{
		// Restore base layer / clear screen.
		clean_screen->CopyTo(screen, 0, 0);

		// Show any revealed ghosts. Reveal ghost if close enough.
		for (Ghost& ghost : ghosts)
		{
			if (ghost.isActive == false) continue;
						
			if (ghost.isFound)
			{
				ShowFoundGhost(ghost);
				CheckForGameOver(ghost);				
			}			
			else
				CheckIfGhostWasFound(ghost);
		}

		// Give player control after starting the game.
		if (isWaitingToStart)
			ShowStartScreen();
		else
		{
			seeker.Draw(mouseX, mouseY); // This is the player.
			ShowScore();
		}		
	}


	// -----------------------------------------------------------
	// Draw ghosts on screen once found
	// -----------------------------------------------------------
	void Game::ShowFoundGhost(Ghost& ghost)
	{
		// Fade in.
		ghost.IncreaseOpacity();
		ghost.sprite->Draw(screen, ghost.m_position.x, ghost.m_position.y, true, ghost.opacity);
	}


	// -----------------------------------------------------------
	// Do a one-time check when ghost is revelaed if game over
	// -----------------------------------------------------------
	void Game::CheckForGameOver(Ghost& ghost)
	{
		// At full opacity, fully reveal ghost and check game state.
		if (ghost.opacity >= 1.0f && ghost.isRevealed == false)
		{
			ghost.isRevealed = true;

			if (GetIsGameOver())
				EndGame();
		}
	}


	// -----------------------------------------------------------
	// If close enough to ghost, find it
	// -----------------------------------------------------------
	void Game::CheckIfGhostWasFound(Ghost& ghost)
	{
		// Credit: Robert Grigg (https://www.3dgep.com/cpp-fast-track-16-physics/) 
		int x_dist = mouseX - ghost.m_center.x;
		int y_dist = mouseY - ghost.m_center.y;
		if (sqrtf(x_dist * x_dist + y_dist * y_dist) < ghost_detection_range)
			GhostFound(ghost);
	}


	// -----------------------------------------------------------
	// Start menu/screen
	// -----------------------------------------------------------
	void Game::ShowStartScreen()
	{
		screen->AlphaBar(0, screen->GetHeight() / 6 * 2, screen->GetWidth(), screen->GetHeight() / 6 * 4, TRANSPARENT_BLACK, 0.5f);
		screen->Bar(0, screen->GetHeight() / 7 * 3, screen->GetWidth(), screen->GetHeight() / 7 * 4, TRANSPARENT_BLACK);
		screen->Centre("C L I C K - T O - S T A R T", screen->GetHeight() / 2, GREEN);
	}


	// -----------------------------------------------------------
	// Show how many ghosts are in the game and progress
	// -----------------------------------------------------------
	void Game::ShowScore()
	{
		int scoreboard_width = static_cast<int>(ghosts.size());
		screen->Bar(ScreenWidth - 5 - (10 * scoreboard_width), ScreenHeight - 20, ScreenWidth, ScreenHeight, BLACK);

		for (int g{ 0 }; g < ghosts.size(); g++)
		{
			if (ghosts[g].isActive)
			{
				int x = ScreenWidth - (10 * (g + 1)), y = ScreenHeight - 12;

				if (ghosts[g].isFound)
					screen->Print("O", x, y, GREEN);
				else
					screen->Print("X", x, y, RED);
			}
		}
	}
};