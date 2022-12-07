#pragma once

#include "surface.h"
#include "template.h"

namespace Tmpl8
{
	class Ghost
	{
	public:
		// METHODS
		Ghost(Sprite* _sprite);
		void UpdateBoundaries(vec2 position);
		void IncreaseOpacity();
		void SetInactive();
		void Reset();

		// ATTRIBUTES
		Sprite* sprite;
		vec2 m_center;
		vec2 m_position;
		const int id;
		int top{ -1 };
		int bottom{ -1 };
		int left{ -1 };
		int right{ -1 };		
		bool isFound{ false };
		bool isRevealed{ false };
		bool isActive{ false };
		float opacity{ 0.0f };

	private:
		// METHODS
		void ResetBoundaries();

		// ATTRIBUTES
		static int ghost_id;
		static float opacity_increase_amount;
	};
};