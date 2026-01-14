#include <cstdint>
#include <ios>
#include <print>
#include <algorithm>
#include <iterator>
#include <random>
#include <set>

#include <rapidcsv.h>

#include "markov.h"
#include "word.h"

using namespace Cq;

int main(int argc, char *argv[])
{
	// if(argc < 2)
	// {
	// 	std::println("Enter a dictionary filename!!!");
	// 	return -1;
	// }

#ifdef SHOULD_USE_CLEANED_DICT
	const auto DICT_FILENAME = "cleaned-csvdict.csv";
#else
	const auto DICT_FILENAME = "csvdict.csv";
#endif

	// const auto DICT_FILENAME = argv[1];
	//	const auto DICT_FILENAME = "testdict.csv";
	//	const auto DICT_FILENAME = "dict.csv";
	std::println("Using {} dictionary file", DICT_FILENAME);
	rapidcsv::Document dictDoc(DICT_FILENAME);

	StdStringVector csvWords = dictDoc.GetColumn<StdString>("word");
	// StdStringVector csvDefs = dictDoc.GetColumn<StdString>("definition");
#ifndef SHOULD_USE_CLEANED_DICT
	for(auto &w : csvWords)
	{
		Helpers::trim(w);
		Helpers::strToLowerCase(w);
	}

	// for(auto &def : csvDefs)
	// {
	// 	Helpers::trim(def);
	// 	Helpers::strToLowerCase(def);
	// }
#endif

	std::println("# of words (non-unique) = {}", csvWords.size());
	Mental::Dictionary dict;
	for(int i = 0; i < csvWords.size(); ++i)
	{
		auto wordName = csvWords[i];
		// auto wordDef = csvDefs[i];
		auto wordDef = dictDoc.GetCell<StdString>("definition", i);

#ifndef SHOULD_USE_CLEANED_DICT
		Helpers::strToLowerCase(wordDef);
#endif

		auto found = dict.find({wordName, {}});
		if(found != dict.end())
		{
			auto newDefs = found->definitions;
			newDefs.push_back(wordDef);
			dict.erase(*found);
			dict.insert({wordName, newDefs});
		} else dict.insert({wordName, {wordDef}});
	}

	std::println("# of dictionary words = {}", dict.size());

	const int N_GRAM = 1;
	Mental::MarkovChain markovChain;
	markovChain.setNGramSize(N_GRAM);
	Mental::Dictionary subDict;
	for(int i = 0; i < 10000; ++i)
	{
		auto w = dict.find({csvWords[i], {}});
		subDict.insert(*w);
	}

	markovChain.setDictionary(subDict);
	// markovChain.setDictionary(dict);
	std::println("# of model states = {}", markovChain.getModel().size());

	std::println("Picking a random state to generate a model from...");
	StdString startingStateStr;
	while(true)
	{
		auto randState = markovChain.getRandomState();

		std::println("Trying {}...", randState.first);
		if(!randState.second.empty())
		{
			startingStateStr = randState.first;
			break;
		}
	}

	std::println("Starting model state = {}", startingStateStr);
	StdString curState = startingStateStr;
	std::println("Initial state = {}", curState);

	if(Helpers::wordCount(curState) != N_GRAM)
	{
		std::println("Error: make sure that the initial curState is the same as N_GRAM!!!");
		return -1;
	}

	StdString text = curState + " ";
	int n = 0;
	const int NUM_ITERS = 10000;
	std::println("Generating text...");
	while(n < NUM_ITERS)
	{
		StdString nextState = markovChain.getPrediction(curState);

		if(nextState == "")
		{
			auto randState = markovChain.getRandomState();
			while(randState.second.empty())
			{
				randState = markovChain.getRandomState();
			}

			curState = randState.first;
			text.append(curState + " ");
			++n;
			continue;
		}

		text.append(nextState + " ");
		curState = nextState;

		++n;
	}

	auto wc = Helpers::wordCount(text);
	std::println("\n\nGenerated text (# of words = {}): \"{}\"", wc, text);

	return 0;
}
