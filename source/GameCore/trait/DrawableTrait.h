#pragma once

#include <SFML/Graphics.hpp>

namespace Game
{
	struct DrawableTraitHandler
	{
		virtual bool initializeGraphics() = 0;
		virtual void onDraw(sf::RenderTarget* const target) = 0;
	};

	class DrawableTraitContainer
	{
	public:
		void pushDrawableTrait(DrawableTraitHandler* const handler)
		{
			drawables.push_back(handler);
		}

		const std::vector<DrawableTraitHandler*>& getDrawableTrait() const
		{
			return drawables;
		}

	protected:
		std::vector<DrawableTraitHandler*> drawables;
	};

	/*
	class TraitContainer
	{
	public:
		void pushTrait(const & )
		{
			.push_back();
		}

		const std::vector<>& getTrait() const
		{
			return ;
		}

	protected:
		std::vector<> ;
	};
	*/
}
