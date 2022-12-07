#include "Ghost.h"

namespace Tmpl8
{
	int Ghost::ghost_id{ 0 };
	float Ghost::opacity_increase_amount{ 0.02f };

	Ghost::Ghost(Sprite* _sprite) :
		id{ghost_id++}
	{
		sprite = _sprite;
		ResetBoundaries();
	}


	// Slowly show ghost after it is found.
	void Ghost::IncreaseOpacity()
	{
		opacity += opacity_increase_amount;
		opacity = (opacity > 1.0f ? 1.0f : opacity);
	}

	
	// Remove from current game.
	void Ghost::SetInactive()
	{
		isActive = false;
		ResetBoundaries();
	}
	
	
	// Reset for next game.
	void Ghost::Reset()
	{
		opacity = 0.0f;
		isFound = false;
		isRevealed = false;
		isActive = true;
		ResetBoundaries();
	}


	// Place on ghost_screen.
	void Ghost::UpdateBoundaries(vec2 center)
	{
		m_center = center;

		top = center.y - (sprite->GetHeight() / 2);
		bottom = center.y + (sprite->GetHeight() / 2);
		left = center.x - (sprite->GetWidth() / 2);
		right = center.x + (sprite->GetWidth() / 2);

		m_position.x = left;
		m_position.y = top;
	}


	// Place ghost offscreen, ready to be placed on ghost_screen.
	void Ghost::ResetBoundaries()
	{		
		top = -1, bottom = -1, left = -1, right = -1;
	}


	
};