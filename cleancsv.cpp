#include <string>
#include <fstream>
#include <print>
#include <format>

#include <rapidcsv.h>

#include "helpers.h"
#include "markov.h"

using namespace Cq;

int main(int argc, char *argv[])
{
	const auto DICT_FILENAME = "csvdict.csv";
	std::ofstream outFile("cleaned-csvdict.csv");

	rapidcsv::Document dictDoc(DICT_FILENAME);
	StdStringVector csvWords = dictDoc.GetColumn<StdString>("word");
	StdStringVector csvWordTypes = dictDoc.GetColumn<StdString>("type");
	for(auto &w : csvWords)
	{
		Helpers::trim(w);
		Helpers::strToLowerCase(w);
	}

	for(auto &wt : csvWordTypes)
	{
		Helpers::trim(wt);
		Helpers::strToLowerCase(wt);
	}


	std::println("# of words (non-unique) = {}", csvWords.size());
	Mental::Dictionary dict;

	for(int i = 0; i < csvWords.size(); ++i)
	{
		auto wordName = csvWords[i];
		auto wordType = csvWordTypes[i];
		auto def = dictDoc.GetCell<StdString>("definition", i);
		Helpers::trim(def);
		Helpers::strToLowerCase(def);
		auto found = dict.find({wordName, {}});
		if(found != dict.end())
		{
			auto newDefs = found->definitions;
			newDefs.push_back(def);
			dict.erase(*found);
			dict.insert({wordName, newDefs, wordType});
		} else dict.insert({wordName, {def}, wordType});
	}

	std::println("{}", dict.size());

	outFile << "word,type,definition\n";

	for(auto entry : dict)
	{
		for(auto def : entry.definitions)
		{
			auto newEntry = std::format("\"{}\",\"{}\",\"{}\"\n", entry.name, entry.types, def);
			outFile << newEntry;
		}
	}

	return 0;
}