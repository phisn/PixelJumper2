#pragma once

#include "trait/CollidableTrait.h"
#include "trait/DrawableTrait.h"
#include "trait/DynamicTrait.h"
#include "trait/DynamicWorldEntry.h"
#include "trait/DynamicWorldExit.h"
#include "trait/ExitableTrait.h"
#include "trait/InitializableTrait.h"
#include "trait/StaticVisibleTrait.h"

#include <vector>
#include <typeinfo>
#include <typeindex>

namespace Game
{
	struct EnvironmentView
		:
		public CollidableTraitContainer,
		public DrawableTraitContainer,
		public DynamicTraitContainer,
		public DynamicWorldEntryContainer,
		public DynamicWorldExitContainer,
		public ExitableTraitContainer,
		public InitializableTraitContainer,
		public StaticVisibleTraitContainer
	{
	};
}
