#pragma once

#include "OperatorClient/OperatorClient.h"

namespace Operator
{
	class CommonRequestInterface
	{
	public:
		bool isFinished() const
		{
			return finished;
		}

		bool isSuccess() const
		{
			return success;
		}

	protected:
		void finish(const bool success)
		{
			this->success = success;
			finished = true;
		}

	private:
		bool finished = false;
		bool success = false;
	};

	inline bool AwaitSyncRequest(
		CommonRequestInterface* const request,
		const sf::Time interval = sf::milliseconds(50))
	{
		sf::Clock clock;
		while (!request->isFinished())
		{
			Operator::Client::Process(clock.restart());
			Net::Core::Process();
			sf::sleep(interval);
		}

		return request->isSuccess();
	}
}
