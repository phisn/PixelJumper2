#include "GenerateWorldImage.h"

namespace Game
{
	sf::Image GenerateWorldImage(Resource::World* worldResource)
	{
		World world{ worldResource };

		if (!world.initialize())
		{
			Log::Error(L"failed to initialize world to generate world image");

			return sf::Image{ };
		}

		if (!world.initializeGraphics())
		{
			Log::Error(L"failed to initialize graphics to generate world image");

			return sf::Image{ };
		}

		sf::RenderTexture renderTexture;
		sf::View view;

		sf::FloatRect viewWindow = world.getInformation()->defaultPlayerProperties.viewWindow;

		view.setSize(viewWindow.width, viewWindow.height);
		view.setRotation(world.getInformation()->defaultPlayerProperties.viewRotation);

		if (world.getInformation()->defaultPlayerProperties.viewFollow)
		{
			view.setCenter(world.getInformation()->defaultPlayerProperties.position);
		}
		else
		{
			view.setCenter(
				viewWindow.left - viewWindow.width / 2.f,
				viewWindow.top - viewWindow.height / 2.f);
		}

		world.getEnvironment()->draw(&renderTexture);
		return renderTexture.getTexture().copyToImage();
	}
}
