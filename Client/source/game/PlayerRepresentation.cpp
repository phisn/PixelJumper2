#include "PlayerRepresentation.h"

namespace Game
{
	PlayerRepresentation* PlayerRepresentation::Create(
		const PlayerInformation& information)
	{
		return new CommonPlayerRepresentation(
			information.color,
			sf::Color::Color(information.color.toInteger() * 0.5)
		);
	}
}
