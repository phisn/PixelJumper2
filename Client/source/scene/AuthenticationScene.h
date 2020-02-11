#pragma once

#include <Client/source/operator/OperatorAccess.h>
#include <Client/source/scene/MenuBaseScene.h>

namespace Scene
{
	class TestMenuScene
		:
		public Operator::AuthenticationEventHandler,
		public MenuBaseScene
	{
	public:
		bool onCreate() override
		{
			const std::string username = "user1";

			char hash[20] = 
			{ 
				0x28, 0xa1, 0xad, 0xb3, 0x99, 
				0xac, 0x19, 0x0b, 0x8e, 0xe9, 
				0xbb, 0x14, 0xd4, 0x4a, 0xa5, 
				0xa2, 0x86, 0x25, 0x51, 0x3f
			};
			
			Operator::AuthenticateCredentials(hash, username);
		}

		void onScenePopped(const int size) override
		{
		}

		void initialize() override
		{
		}

		void onLogic(const sf::Time time) override
		{
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		void onConnected() override
		{
		}
	};
}
