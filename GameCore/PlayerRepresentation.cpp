#include "PlayerRepresentation.h"

namespace Game
{
	PlayerRepresentation* PlayerRepresentation::Create(
		const PlayerInformation& information)
	{
		return new CommonPlayerRepresentation(
			sf::Color::Red,
			sf::Color::Red
		);
	}
}
