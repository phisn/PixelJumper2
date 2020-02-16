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
