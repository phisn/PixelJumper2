#include "OperatorConnectionHandler.h"

Operator::ConnectionHandler* Operator::ConnectionHandler::instance = NULL;
sf::Time Operator::ConnectionHandler::processDelay;
sf::Time Operator::ConnectionHandler::processCounter = sf::Time::Zero;
