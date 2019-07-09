#pragma once

#include <Client/source/framework/Context.h>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace Framework
{
	namespace
	{
		static bool running = true;
	}

	namespace Interface
	{
		static void Draw(sf::RenderTarget* const target)
		{
			Context::GetStack().back()->draw(target);
		}

		static void Event(const sf::Event event)
		{
			Context::GetStack().back()->Event(event);
		}

		static void Update(const sf::Time time)
		{
			Context::GetStack().back()->update(time);
		}

		static void ProcessTask()
		{
			Context::ProcessTask();
		}

		static bool IsRunning()
		{
			return running;
		}

		static void Shutdown()
		{
			running = false;
		}

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
