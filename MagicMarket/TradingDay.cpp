#include "TradingDay.h"

#include "Stock.h"
#include "Market.h"

namespace MM
{


	TradingDay::TradingDay(QuantLib::Date date, Stock *stock) : date(date), stock(stock)
	{
		saveFile = nullptr;
	}


	TradingDay::~TradingDay()
	{
		if (saveFile)
		{
			saveFile->close();
			delete saveFile;
			saveFile = nullptr;
		}
	}

	std::string TradingDay::getCurrencyPair()
	{
		assert(stock);
		return stock->getCurrencyPair();
	}

	TradingDay *TradingDay::getPreviousDay()
	{
		return stock->getTradingDay(date - 1);
	}

	TradingDay *TradingDay::getNextDay()
	{
		return stock->getTradingDay(date + 1);
	}

	void TradingDay::serializeTick(const Tick &tick)
	{
		getSaveFile() << tick << std::flush;
	}

	void TradingDay::receiveFreshTick(const Tick &tick)
	{
		// check last tick if time is equal
		if (ticks.size())
		{
			if (ticks.back().time == tick.time)
			{
				ticks.back() = tick;
				// overwrite the old tick in the savefile..
				if (!market.isVirtual())
				{
					getSaveFile().seekp(-tick.getOutputBitSize(), std::ios_base::cur);
					getSaveFile() << tick << std::flush;
				}
				return;
			}
		}

		ticks.push_back(tick);
		// save the tick!
		if (!market.isVirtual())
			serializeTick(tick);
	}

	std::string TradingDay::getSaveFileName()
	{
		return TradingDay::getSaveFileName(date);
	}

	std::string TradingDay::getSaveFileName(QuantLib::Date forDate)
	{
		std::ostringstream os;
		os << forDate.year() << "-" << (int)forDate.month() << "-" << forDate.dayOfMonth() << ".ticks";
		return os.str();
	}

	std::string TradingDay::getSavePath()
	{
		return TradingDay::getSavePath(stock, date);
	}

	std::string TradingDay::getSavePath(Stock *stock, QuantLib::Date forDate)
	{
		std::ostringstream os;
		os << stock->getDirectoryName() << "/" << getSaveFileName(forDate);
		return os.str();
	}

	std::ostream &TradingDay::getSaveFile()
	{
		assert(!market.isVirtual());
		if (saveFile) return *saveFile;

		std::string filename = getSavePath();
		saveFile = new std::fstream(filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::app);

		return *saveFile;
	}

	bool TradingDay::loadFromFile()
	{
		std::string filename = getSavePath();
		std::fstream file(filename.c_str(), std::ios_base::in | std::ios_base::binary);

		if (!file.good()) return false;

		while (file.good())
		{
			Tick newTick;
			file >> newTick;
			ticks.push_back(newTick);
		}

		file.close();
		return true;
	}
};