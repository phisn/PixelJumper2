#pragma once

namespace Game
{
	class World;
	class InitializableTraitHandler
	{
	public:
		// can fail
		virtual bool initialize(World* const world) = 0;
	};

	class InitializableTraitContainer
	{
	public:
		void pushInitializableTrait(InitializableTraitHandler* const initializeable)
		{
			initializeables.push_back(initializeable);
		}

		const std::vector<InitializableTraitHandler*>& getInitializableTrait() const
		{
			return initializeables;
		}

	protected:
		std::vector<InitializableTraitHandler*> initializeables;
	};
}
