#pragma once

#include <vector>

#include "surface.h"
#include "ghost.h"


namespace Tmpl8
{
	struct mouseCoordinates
	{
		int x{ 0 };
		int y{ 0 };
	};

	class Seeker
	{
	public:
		// METHODS
		Seeker();
		void Draw(int mouse_x, int mouse_y);
		void SetSurface(Surface* screen);
		void SetGhostInfo(Surface* screen, std::vector<Ghost>* ghosts);
		
	private:
		// METHODS
		void UpdateBoundaries();
		void DetectGhosts();
		void RevealGhost(int x1, int y1, int width, int height);
		void DrawCircle();		

		// ATTRIBUTES
		Surface* m_screen{ nullptr };
		Surface* m_ghost_screen{ nullptr };
		Surface opacity_map;
		mouseCoordinates mouse;
		std::vector<Ghost>* m_ghosts;
		int radius{ 64 };
		int m_top{ 0 };
		int m_bottom{ 0 };
		int m_left{ 0 };
		int m_right{ 0 };
	};
};

