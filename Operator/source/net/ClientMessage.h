#pragma once

#include <Client/source/game/net/NetworkMessage.h>
#include <Operator/source/Common.h>

namespace Operator::Net::Client
{
	// authentication
	// 1. client hashes password without salt
	// 2. client send hashed password to operator
	// 3. operator hashes hash with stored salt
	// 4. operator compares hashed hash with
	//    stored hash

	struct AuthMessageID
	{
		enum
		{
			_Begin = Game::Net::CommonMessageID::_Offset - 1,

			// request salt to hash password for
			// login
			RequestLogin,

			// login with hash after received salt
			// from requestlogin
			Authenticate,

			// create a new player with valid
			// key
			Register,

			_Offset
		};
	};

	struct AuthenticationMessage
		:
		public Game::Net::NetworkMessage
	{
		std::string username;

		struct Content
		{
			char hash[OPERATOR_HASH_SIZE];

		} content;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readString(&username)
				&& pipe->readValue(&content);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeString(&username)
				&& pipe->writeValue(&content);
		}
	};

	struct RegistrationMessage
		:
		public Game::Net::NetworkMessage
	{
		std::string username;

		struct Content
		{
			char hash[OPERATOR_HASH_SIZE];
			char key[OPERATOR_KEY_SIZE];

		} content;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readString(&username)
				&& pipe->readValue(&content);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeString(&username)
				&& pipe->writeValue(&content);
		}
	};
}
