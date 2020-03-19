#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/AuthenticationRequest.h"

#include <iostream>

struct AuthenticationRequestHandler
	:
	public Operator::ClientAuthenticationRequest
{
	void onAuthenticated(const Operator::UserID userID) override
	{
		Log::Information(L"onauthenticated without hash", userID, L"userID");
	}

	void onAuthenticated(
		const char token[OPERATOR_HASH_SIZE],
		const Operator::UserID userID) override
	{
		Log::Information(L"onauthenticated with hash", userID, L"userID");
	}
	
	void onAuthenticationFailed(const ClientAuthenticationRequest::Reason reason) override
	{
		Log::Information(L"onauthenticationfailed", (int)reason, L"reason");
	}
	
	void onRegistrationFailed(const Net::Host::RejectRegistrationMessage::Reason reason) override
	{
		Log::Information(L"onregistrationfailed", (int)reason, L"reason");
	}

	void onRequestFailure(const RequestInterface::Reason reason) override
	{
		Log::Information(L"onrequestfailed", (int)reason, L"reason");
		ClientAuthenticationRequest::onRequestFailure(reason);
	}
};

int main()
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);

	if (!Net::Core::Initialize())
	{
		Log::Error(L"failed to initialize net core");

		std::cin.ignore();
		return 0;
	}

	SteamNetworkingIPAddr address;
	address.SetIPv6LocalHost(9928);
	Operator::Client::Initialize(address);

	Net::Client::AuthenticationMessage* message = new Net::Client::AuthenticationMessage{ };
	message->username = "test";
	AuthenticationRequestHandler handler;
	
	Module::Encryption::HashCommon(
		(unsigned char*) message->content.hash,
		(unsigned char*) message->username.c_str(),
		message->username.size());

	Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
		Net::Client::OperatorAuthenticationMessageID::Authenticate,
		message,
		&handler);

	if (result != Operator::Client::PushRequestFailure::Success)
	{
		Log::Error(L"Operator request failed", (int) result, L"result");

		std::cin.ignore();
		return 0;
	}

	sf::Clock clock;
	while (true)
	{
		Net::Core::Process();
		Operator::Client::Process(clock.restart());

		sf::sleep(sf::milliseconds(50));
	}

	std::cin.ignore();
	return 0;
}
