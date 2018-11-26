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
			Context* context);
		bool PopContext();
		
		bool PushScene(
			Scene::SubBase* scene);
		bool PopScene();

		void Fallback();

		void PushAsyncAnimation(
			AsyncAnimation* animation);
		void PushSequentialAnimation(
			Animation* animation);

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
			RESOURCE::Static::Type type);
	}
}
