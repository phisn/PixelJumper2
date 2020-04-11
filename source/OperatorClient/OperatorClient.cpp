#include "OperatorClient.h"

sf::Time Operator::Client::processInterval;
sf::Time Operator::Client::processCounter;

Operator::Client* Operator::Client::client = NULL;

Util::Notifier<Operator::Client, bool> Operator::Client::OpenConnectionNotifier;
Util::Notifier<Operator::Client, int> Operator::Client::LostConnectionNotifier;
Util::Notifier<Operator::Client, int> Operator::Client::ClosedConnectionNotifier;
