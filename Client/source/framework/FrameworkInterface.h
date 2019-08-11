#pragma once

#include <Client/source/framework/Context.h>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace Framework
{
	namespace Interface
	{
		void Draw(sf::RenderTarget* const target);
		void Event(const sf::Event event);
		void Update(const sf::Time time);

		void ProcessTask();

		bool IsRunning();
		void Shutdown();


		_Ret_maybenull_
		template <typename T>
		const T* GetResource(const Resource::Static::Id type)
		{
			for (Context* const context : Context::GetStack())
				if (context->getResource()->has(type))
				{
					return context->getResource()->read<T>(type);
				}

			return Context::GetStack().back()->getResource()->obtain<T>(type);
		}
	}
}
