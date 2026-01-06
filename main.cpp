#include <cstdint>
#include <ios>
#include <print>
#include <algorithm>
#include <iterator>
#include <random>
#include <set>

#include <rapidcsv.h>

#include "markov.h"

using namespace Cq;

struct Word
{
	StdString name;
	StdStringVector definitions;
};

void strToLowerCase(StdString &str)
{
	for(auto &ch : str) ch = std::tolower(ch);
}

bool operator<(const Word &w1, const Word &w2)
{
	return w1.name < w2.name;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::println("Enter a dictionary filename!!!");
		return -1;
	}

	const auto DICT_FILENAME = argv[1];
	//	const auto DICT_FILENAME = "testdict.csv";
	//	const auto DICT_FILENAME = "dict.csv";
	rapidcsv::Document dictDoc(DICT_FILENAME);

	StdStringVector words = dictDoc.GetColumn<StdString>("word");
	for(auto &w : words) strToLowerCase(w);

	std::println("# of words (non-unique) = {}", words.size());
	std::set<Word, std::less<>> dict;

	for (int i = 0; i < words.size(); ++i)
	{
		auto def = dictDoc.GetCell<StdString>("definition", i);
		strToLowerCase(def);
		auto found = dict.find({words[i], {}});
		if (found != dict.end())
		{
			auto newDefs = found->definitions;
			newDefs.push_back(def);
			dict.erase(*found);
			dict.insert({words[i], newDefs});
		} else dict.insert({words[i], {def}});
	}

	std::println("# of dictionary words = {}", dict.size());

	std::println("Cleaning up definitions...");
	StdMap<StdString, StdStringVector> wordToCleanDefWords;
	for (const auto &w : dict)
	{
		auto cleanDefSentences = Helpers::makeCleanSentences(w.definitions);
		auto cleanDefWords = Helpers::makeCleanWords(cleanDefSentences);
		wordToCleanDefWords[w.name] = cleanDefWords;
	}

	std::println("Done cleaning definitions");

	std::random_device rd;
	std::default_random_engine rne(rd());

	auto getRandDictWord = [&rne, &dict](void) {
		StdVector<Word> randWords;
		std::sample(dict.cbegin(), dict.cend(), std::back_inserter(randWords), 1, rne);
		return randWords[0];
	};

	std::println("Picking a random word to generate a model from...");
	const int N_GRAM = 1;
	Mental::MarkovChain curModel;
	Word startingWord;
	while (true)
	{
		startingWord = getRandDictWord();
		std::println("Trying {}...", startingWord.name);

		curModel.reset(wordToCleanDefWords[startingWord.name], N_GRAM);
		if(!curModel.isEmpty()) break;
	}

	std::println("Starting model word = {}", startingWord.name);
	// std::println("{}", curModel);
	StdString curState = curModel.getRandomState().first;
	std::println("Initial state word = {}", curState);

	if (Helpers::wordCount(curState) != N_GRAM)
	{
		std::println("Error: make sure that the initial curState is the same as N_GRAM!!!");
		return -1;
	}

	StdString text = curState + " ";
	int n = 0;
	const int NUM_ITERS = 500;
	std::println("Generating text...");
	while (n < NUM_ITERS)
	{
		StdString nextState = curModel.getPrediction(curState);

		if (nextState == "")
		{
			curState = curModel.getRandomState().first;
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
