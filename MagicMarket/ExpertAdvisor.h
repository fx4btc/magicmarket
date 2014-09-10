#pragma once

#include <string>
#include <ctime>

#include <ql/types.hpp>
#include <ql/time/date.hpp>

namespace MM
{

	class ExpertAdvisor
	{
	public:
		ExpertAdvisor();
		virtual ~ExpertAdvisor();

		virtual std::string getName() = 0;

		virtual void execute() = 0;
		virtual void onNewTick(const std::string &currencyPair, const QuantLib::Date &date, const std::time_t &time) = 0;


		void say(std::string);
	private:
		std::string lastMessage;
	};

};