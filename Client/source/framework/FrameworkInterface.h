#pragma once

#include <Client/source/resource/StaticResource.h>
#include <Client/source/scene/SubSceneBase.h>

#include <SFML/System/MemoryInputStream.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#ifndef FW
#define FW ::Framework
#endif

namespace Framework
{
	class Animation;
	class AsyncAnimation;

	class Context;

	namespace Interface
	{
		enum class Task
		{
			Empty,

			LoadScene,
			PopScene,

			Fallback, // removes all subscenes

			LoadContext,
			PopContext,

			Termination
		};

		bool PushContext(
			Context* const context);
		bool PopContext();
		
		bool PushScene(
			Scene::SubBase* const scene);
		bool PopScene();

		void Fallback();

		void PushAsyncAnimation(
			AsyncAnimation* const animation);
		void PushSequentialAnimation(
			Animation* const animation);

		void ResetAnimations();
	}

	namespace Execution
	{
		void OnDraw();
		void OnEvent(
			const sf::Event event);
		void OnUpdate(
			const sf::Time time);

		void DoTasks();

		void Shutdown();
		bool IsRunning();
	}

	namespace Resource
	{
		sf::MemoryInputStream Get(
			const RESOURCE::Static::Type type);
	}
}
