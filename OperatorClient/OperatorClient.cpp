#include "OperatorClient.h"

sf::Time Operator::Client::processInterval;
sf::Time Operator::Client::processCounter;

Operator::Client* Operator::Client::client = NULL;
