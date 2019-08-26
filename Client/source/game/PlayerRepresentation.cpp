#include "PlayerRepresentation.h"

namespace Game
{
	PlayerRepresentation* PlayerRepresentation::Create(
		const PlayerInformation& information)
	{
		return new CommonPlayerRepresentation(
			information.color,
			information.color - sf::Color(30, 30, 30, information.color.a / 2)
		);
	}
}
