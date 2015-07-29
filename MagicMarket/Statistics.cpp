#include "Statistics.h"
#include <SimpleIni.h>

#include <sstream>

MM::Statistics statistics;

namespace MM
{

	Statistics::Statistics()
	{
	}


	Statistics::~Statistics()
	{
	}

	void Statistics::init(const CSimpleIniA &ini)
	{
		loggingActive = ini.GetBoolValue("Statistics", "Enabled", true);
		config.outputFilename = ini.GetValue("Statistics", "OutputFilename", "saves/variables.csv");
		config.delimiter = ini.GetValue("Statistics", "Delimiter", "\t");
	}

	void Statistics::addVariable(const Variable &var)
	{
		const int duplicateCount = std::count_if(variables.begin(), variables.end(), [&](const Variable &other) { return other.name == var.name; }); 

		variables.push_back(var);

		if (duplicateCount > 0)
		{
			std::ostringstream os;
			os << var.name << "_" << (duplicateCount + 1);
			variables.back().name = os.str();
		}
	}

	void Statistics::log()
	{
		if (!loggingActive) return;

		// try to open for the very first time?
		if (!outputStream.is_open())
		{
			// log variable descriptions
			std::fstream descriptionStream(config.outputFilename + ".info", std::ios_base::out);
			// open real stream
			outputStream.open(config.outputFilename, std::ios_base::out);
			if (!outputStream.good()) return;

			// print header row
			bool isFirst = true;
			for (const Variable &var : variables)
			{
				if (!isFirst) outputStream << config.delimiter;
				else isFirst = false;
				outputStream << var.name;

				descriptionStream << var.name << config.delimiter << var.description << std::endl;
			}
		}

		// get all observations and log them
		auto print = [&](const Variable &var, bool isFirst)
		{
			if (!isFirst) outputStream << config.delimiter;
			outputStream << var.get();
		};
		for (size_t i = 0; i < variables.size(); ++i)
		{
			print(variables[i], i == 0);
		}
		// flush file to allow killing the application without losses
		outputStream << std::flush;
	}
};