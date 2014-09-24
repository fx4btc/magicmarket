#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <assert.h>
#include <WinSock2.h>
#include <zmq.h>

#include <ql/time/date.hpp>

#include "Account.h"
#include "Event.h"

class zmq_msg_buf
{
public:
	zmq_msg_buf()
	{
		int e = zmq_msg_init(&msg);
		assert(e == 0);
	}
	~zmq_msg_buf()
	{
		int e = zmq_msg_close(&msg);
		assert(e == 0);
	}
	zmq_msg_t msg;
};

namespace MM
{
	class ExpertAdvisor;
	class Trade;
	class Stock;

	class Market
	{
	public:
		Market();
		~Market();
		
		Stock *getStock(std::string, bool allowCreation = false);
		void addStock(std::string);

		std::vector<Trade*> &getOpenTrades() { return trades; };
		Trade *newTrade(Trade trade);
		void updateTrade(Trade *trade);
		void closeTrade(Trade *trade);

		void chat(std::string name, std::string message);
		void updateMood(std::string name, float mood, float certainty);
		void updateParameter(std::string name, double value);

		void init();
		void run();

		const Account& getAccount() { return account; }

		std::string getSaveFolderName() { return "saves"; }

		void addEvent(const Event &e);
		std::vector<ExpertAdvisor*> &getExperts() { return experts; };
	private:
		Account account;

		std::map<std::string, Stock*> stocks;
		std::vector<Trade*> trades;
		std::vector<ExpertAdvisor*> experts;
		void loadConfig();

		friend class Stock;

		// to notify agents
		std::vector<Event> events;

		// for the communication protocol
		void *zmqContext;
		void *zmqListener;
		void *zmqPublisher;
		std::string getCommandPrefix();
		std::string connectionStringListener, connectionStringSpeaker;
		std::string accountName, uid;

		void setupConnection();
		void connectToCentralQueue();
		void send(std::string, int probabilityToSendInVirtualMode = 100);
		std::string receive();
		void parseMessage(const std::string &message);

		// Interface for the virtual market
	private:
		friend class VirtualMarket;
		std::chrono::milliseconds sleepDuration;
		bool isVirtualModeEnabled;
		
		void setSleepDuration(int ms);
		void setVirtual(bool state) { isVirtualModeEnabled = state; };
	public:
		bool isVirtual() { return isVirtualModeEnabled; }
	};
};

extern MM::Market market;