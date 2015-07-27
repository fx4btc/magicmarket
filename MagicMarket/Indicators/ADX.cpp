#include "ADX.h"
#include "Market.h"
#include "Stock.h"
#include "TimePeriod.h"

#include "Helpers.h"

#include "Moves.h"
#include "ATR.h"

namespace MM
{
	namespace Indicators
	{

		ADX::ADX(std::string currencyPair, int history, int seconds) : 
			currencyPair(currencyPair),
			history(history),
			seconds(seconds)
		{
			pDIMA = std::numeric_limits<double>::quiet_NaN();
			mDIMA = std::numeric_limits<double>::quiet_NaN();
			adx = std::numeric_limits<double>::quiet_NaN();

			moves = static_cast<Moves*>(Moves(currencyPair, history, seconds).init());
			atr   = static_cast<ATR*>  (ATR(currencyPair, history, seconds).init());
		}


		ADX::~ADX()
		{
		}

		void ADX::update(const std::time_t &secondsSinceStart, const std::time_t &time)
		{
			const double pDI = 100.0 * moves->getPlusDMMA()  / atr->getATRMA();
			const double mDI = 100.0 * moves->getMinusDMMA() / atr->getATRMA();

			if (std::isnan(pDIMA))
				pDIMA = pDI;
			else
				pDIMA = Math::MA(pDIMA, pDI, history);

			if (std::isnan(mDIMA))
				mDIMA = mDI;
			else
				mDIMA = Math::MA(mDIMA, mDI, history);

			adx = 100.0 * std::abs(pDIMA - mDIMA) / (pDI + mDI);
		}
	};
};