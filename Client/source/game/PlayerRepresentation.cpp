#include "PlayerRepresentation.h"

namespace Game
{
	PlayerRepresentation* PlayerRepresentation::Create(
		const PlayerInformation& information)
	{
		return new CommonPlayerRepresentation(
			information.color,
			sf::Color(
				information.color.r * 0.8f,
				information.color.g * 0.8f,
				information.color.b * 0.8f,
				information.color.a * 0.8f
			)
		);
	}
}
