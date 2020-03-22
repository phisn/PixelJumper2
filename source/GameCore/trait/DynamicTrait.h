#pragma once

#include <vector>

namespace Game
{
	struct DynamicTrait
	{
		virtual void processLogic() = 0;
	};

	class DynamicTraitContainer
	{
	public:
		void pushDynamicTrait(DynamicTrait* const dynamic)
		{
			dynamics.push_back(dynamic);
		}

		const std::vector<DynamicTrait*>& getDynamicTrait() const
		{
			return dynamics;
		}

	protected:
		std::vector<DynamicTrait*> dynamics;
	};
}
