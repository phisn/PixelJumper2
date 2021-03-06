#pragma once

#include "NetCore/NetworkMessage.h"
#include "OperatorRequestMessage.h"

#include "Common/Common.h"
#include "ResourceCore/PlayerResource.h"

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

namespace Net::Client
{
	// authentication
	// 1. client hashes hash without salt
	// 2. client send hashed hash to operator
	// 3. operator hashes hash with stored salt
	// 4. operator compares hashed hash with
	//    stored hash
	struct OperatorAuthenticationMessageID
	{
		enum
		{
			_Begin = OperatorRequestMessageID::_Offset - 1,

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

	struct OperatorAuthenticationMessage
		:
		public ::Net::NetworkMessage
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

	struct OperatorTokenMessageContent
	{
		char token[OPERATOR_HASH_SIZE];
	};

	typedef ::Net::TrivialNetworkMessage<OperatorTokenMessageContent> OperatorTokenMessage;

	struct OperatorRegistrationMessage
		:
		public ::Net::NetworkMessage
	{
		std::string username;

		struct Content
		{
			char hash[OPERATOR_HASH_SIZE];
			Operator::RegistrationKey key;

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

namespace Net::Host
{
	struct ConnectionClosedReason
	{
		enum
		{
			IdleConnection,
			Authentication
		};
	};

	struct OperatorAuthenticationMessageID
	{
		enum
		{
			_Begin = OperatorRequestMessageID::_Offset - 1,

			Timeout,

			AcceptRegistration,
			RejectRegistration,

			// similiar like normal authentication
			// but with token
			AcceptToken,
			AcceptAuthentication,

			// authentication failure for token and
			// general authentication
			AuthenticationFailure,

			_Offset
		};
	};

	struct AcceptOperatorAuthenticationMessageContent
	{
		Operator::UserID userID;
		char authenticationToken[OPERATOR_HASH_SIZE];
	};

	typedef ::Net::TrivialNetworkMessage<AcceptOperatorAuthenticationMessageContent> AcceptOperatorAuthenticationMessage;

	struct AcceptOperatorTokenMessageContent
	{
		Operator::UserID userID;
	};

	typedef ::Net::TrivialNetworkMessage<AcceptOperatorTokenMessageContent> AcceptOperatorTokenMessage;

	enum class AuthenticationFailureReason
	{
		AlreadyOnline,
		// alreadyhosting only called when trying to login
		// with cred because this would cause the recreation of
		// the authentication token
		AlreadyHosting,
		AuthenticationRejected
	};

	struct AuthenticationFailureMessageContent
	{
		AuthenticationFailureReason reason;
	};

	typedef TrivialNetworkMessage<AuthenticationFailureMessageContent> AuthenticationFailureMessage;

	struct AcceptOperatorRegistrationMessageContent
	{
		Operator::UserID userID;
		char authenticationToken[OPERATOR_HASH_SIZE];
	};

	typedef ::Net::TrivialNetworkMessage<AcceptOperatorRegistrationMessageContent> AcceptOperatorRegistrationMessage;

	struct RejectOperatorRegistrationMessageContent
	{
		enum Reason
		{
			UsernameUsed,
			KeyUsed,
			KeyInvalid,
			InternalError

		} reason;
	};

	typedef ::Net::TrivialNetworkMessage<RejectOperatorRegistrationMessageContent> RejectOperatorRegistrationMessage;
}
