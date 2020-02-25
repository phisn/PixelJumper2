#pragma once

#include <Client/source/game/net/NetworkMessage.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/Common.h>

/*
	
	this process is deprecated:
		the new process hashes the player and host token. this
		hash is then sent to the host which is then sending it
		to the operator to validate it. the operator is the
		only on who does know both tokens

Authentication:
	client and server authenticate identically
	the hash is hashed at clientside without
	salt and is send to the server. the server
	hashes the hash with a salt and compares it
	to a version in the database. if they are
	the same then the connection is accepted.
	the server requests the server host games
	with infos. the server saves and accept the
	host. the client requets to join a server
	and the operator starts to either find or
	directly push the client to the specified 
	server. this server than can accept the
	client and saves an random authentication
	id. the operator now tells the client
	to join the server with that authentication
	id. the client joins the server with the
	authentication id. the server compares both
	ids and accepts the connection

	C         O          S
	| Auth    | Auth     |
	|\-------\|/--------/|
	| Accept  | Accept   |
	|/-------/|\--------\|
	|         | Host     |
	|         |/--------/|
	|         | Accept   |
	|         |\--------\|
	| Join    |          |
	|\-------\| Push     |
	|         |\--------\|
	|         | Accept   |
	| Put     |/--------/|
	|/-------/|          |
	|         x          |
	| Join               |
	|\------------------\|
	| Accept             |
	|/------------------/|


*/

namespace Operator::Net::Client
{
	// authentication
	// 1. client hashes hash without salt
	// 2. client send hashed hash to operator
	// 3. operator hashes hash with stored salt
	// 4. operator compares hashed hash with
	//    stored hash
	struct AuthMessageID
	{
		enum
		{
			_Begin = Game::Net::CommonMessageID::_Offset - 1,

			// login with hash after received salt
			// from requestlogin
			Authenticate,

			// authentication with token instead of
			// login
			Token,

			// create a new player with valid key
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

	struct TokenMessageContent
	{
		char token[OPERATOR_HASH_SIZE];
	};

	typedef Game::Net::TrivialNetworkMessage<TokenMessageContent> TokenMessage;

	struct RegistrationMessage
		:
		public Game::Net::NetworkMessage
	{
		std::string username;

		struct Content
		{
			char hash[OPERATOR_HASH_SIZE];
			RegistrationKey key;

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

namespace Operator::Net::Host
{
	struct ConnectionClosedReason
	{
		enum
		{
			IdleConnection,
			Authentication
		};
	};

	struct AuthMessageID
	{
		enum
		{
			_Begin = Game::Net::CommonMessageID::_Offset - 1,

			InternalError,
			Timeout,

			// closes connection to accept new ones
			IdleConnection,

			AcceptRegistration,
			RejectRegistration,

			// similiar like normal authentication
			// but with token
			AcceptToken,
			RejectToken,

			AcceptAuthentication,

			// reject authentication does not specify
			// reason to prevent information gathering
			// of the database
			RejectAuthentication,

			_Offset
		};
	};

	struct AcceptAuthenticationMessageContent
	{
		UserID userID;
		char authenticationToken[OPERATOR_HASH_SIZE];
	};

	typedef Game::Net::TrivialNetworkMessage<AcceptAuthenticationMessageContent> AcceptAuthenticationMessage;

	struct AcceptTokenMessageContent
	{
		UserID userID;
	};

	typedef Game::Net::TrivialNetworkMessage<AcceptTokenMessageContent> AcceptTokenMessage;

	struct AcceptRegistrationMessageContent
	{
		UserID userID;
		char authenticationToken[OPERATOR_HASH_SIZE];
	};

	typedef Game::Net::TrivialNetworkMessage<AcceptRegistrationMessageContent> AcceptRegistrationMessage;

	struct RejectRegistrationMessageContent
	{
		enum Reason
		{
			UsernameUsed,
			KeyUsed,
			KeyInvalid,
			InternalError

		} reason;
	};

	typedef Game::Net::TrivialNetworkMessage<RejectRegistrationMessageContent> RejectRegistrationMessage;
}
