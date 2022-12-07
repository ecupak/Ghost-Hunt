#include <cstdint> //small int for opacity extraction

#include "template.h"
#include "seeker.h"


namespace Tmpl8
{
	// Constructor(s).
	Seeker::Seeker() :
		opacity_map{ Surface("assets/opacity-orb.png") }
	{
		// opacity_map should ideally be 2^n pixels & width == height.
		radius = opacity_map.GetPitch() / 2;
	}


	// Set display Surface.
	void Seeker::SetSurface(Surface* screen)
	{
		m_screen = screen;
	}


	// Set Surface ghosts are drawn to and reference to ghost list.
	void Seeker::SetGhostInfo(Surface* screen, std::vector<Ghost>* ghosts)
	{
		m_ghost_screen = screen;
		m_ghosts = ghosts;
	}


	// Draw seeker and any ghosts revealed within it.
	void Seeker::Draw(int mouse_x, int mouse_y)
	{
		// Nothing to draw to.
		if (m_screen == nullptr || m_ghost_screen == nullptr) return;

		// Get coordinates.
		mouse.x = mouse_x, mouse.y = mouse_y;

		// Set bounding box.
		UpdateBoundaries();

		// Show ghosts if any.
		DetectGhosts();

		// Show detection circle.
		DrawCircle();
	}


	// Create a bounding box.
	void Seeker::UpdateBoundaries()
	{
		m_top = mouse.y - radius;
		m_bottom = mouse.y + radius;
		m_left = mouse.x - radius;
		m_right = mouse.x + radius;
	}


	void Seeker::DetectGhosts()
	{
		for (auto& ghost : *m_ghosts)
		{
			if (ghost.isFound) continue;

			// Check if box overlaps hidden ghost.
			// Credit: https://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other
			if (m_left < ghost.right && m_right > ghost.left && m_top < ghost.bottom && m_bottom > ghost.top)
			{
				// Find the boundary of the overlap.
				int top = (m_top < ghost.top ? ghost.top : m_top);
				int bottom = (m_bottom < ghost.bottom ? m_bottom : ghost.bottom);
				int left = (m_left < ghost.left ? ghost.left : m_left);
				int right = (m_right < ghost.right ? m_right : ghost.right);

				// Find size of the overlap.
				int width = right - left;
				int height = bottom - top;

				// Reveal the ghost in the overlap.
				RevealGhost(left, top, width, height);
			}
		}
	}


	// Uses opacity map and ghostRealm Surface to copy any overlapping ghosts with alpha effect.
	void Seeker::RevealGhost(int left, int top, int width, int height)
	{		
		// Prepare start of the display and ghost screen Pixel pointers.
		Pixel* d = m_screen->GetBuffer() + left + (top * m_screen->GetPitch());
		Pixel* g = m_ghost_screen->GetBuffer() + left + (top * m_ghost_screen->GetPitch());

		// Get start of the opacity mapping.
		int offset_x = (left == m_left ? 0 : left - m_left);
		int offset_y = (top == m_top ? 0 : top - m_top);		
		Pixel* o = opacity_map.GetBuffer() + offset_x + (offset_y * opacity_map.GetPitch());
		
		// Copy ghosts with alpha to main display.
		int right = left + width, bottom = top + height;
		for (int y{ top }; y <= bottom; y++)
		{
			// Confirm we are on the screen.
			if (y < 0 || y >= m_screen->GetHeight()) continue;

			for (int x{ 0 }; x < (right - left); x++)
			{
				// Make sure we are on the screen.
				if (x < 0 || x + left >= m_screen->GetWidth()) continue;

				// First 2 bytes contain opacity information. (My image software does not save alpha channel - this is a hack)
				// This gives us a value between 0 and 255. Worried about speed here, so asking for the fastest 8-bit-miniumum unsigned int we can get.
				std::uint_fast8_t opacity_ref = o[x] & 255;

				// Show the ghost at mapped opacity over the main display.
				if (opacity_ref > 0)
				{
					d[x] = MixAlpha(g[x], opacity_ref / 255.0f, d[x]); 					
				}
				//d[x] = g[x];
			}
			d += m_screen->GetPitch();
			g += m_ghost_screen->GetPitch();
			o += opacity_map.GetPitch();
		}
	}

	
	// Circle outline.
	void Seeker::DrawCircle()
	{
		// Credit: Robert Grigg (https://www.3dgep.com/cpp-fast-track-16-physics/)			
		float r0 = radius + 2;
		for (int i = 0; i < 64; i++)
		{
			float r1 = (float)i * PI / 32, r2 = (float)(i + 1) * PI / 32;
			m_screen->Line(mouse.x - r0 * sinf(r1), mouse.y - r0 * cosf(r1),
				mouse.x - r0 * sinf(r2), mouse.y - r0 * cosf(r2), 0xCCCC00);
		}
	}	
};