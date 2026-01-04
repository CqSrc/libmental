#include <cstdint>
#include <ios>
#include <print>
#include <algorithm>
#include <iterator>
#include <random>
#include <set>

#include <rapidcsv.h>

#include "helpers.h"
#include "markov.h"

struct Word
{
	StdString name;
	StdStringVector definitions;
};

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

	std::println("# of words (non-unique) = {}", words.size());
	std::set<Word, std::less<>> dict;

	for (int i = 0; i < words.size(); ++i)
	{
		auto def = dictDoc.GetCell<StdString>("definition", i);
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
		auto cleanDefSentences = Cq::Helpers::makeCleanSentences(w.definitions);
		auto cleanDefWords = Cq::Helpers::makeCleanWords(cleanDefSentences);
		wordToCleanDefWords[w.name] = cleanDefWords;
	}

	std::println("Done cleaning definitions");

	std::random_device rd;
	std::default_random_engine rne(rd());

	const int N_GRAM = 2;
	auto getRandDictWord = [&rne, &dict](void) {
		StdVector<Word> randWords;
		std::sample(dict.cbegin(), dict.cend(), std::back_inserter(randWords), 1, rne);
		return randWords[0];
	};

	auto getRandModel = [&rne, &wordToCleanDefWords, &getRandDictWord](void) {
		auto randWord = getRandDictWord();
		return std::make_pair(randWord, Cq::Mental::makeMarkovModel(wordToCleanDefWords[randWord.name], N_GRAM));
	};

	auto getRandState = [&rne](const Cq::Mental::MarkovModel &model) {
		Cq::Mental::MarkovModelStatePairVector randStates;
		std::sample(model.cbegin(), model.cend(), std::back_inserter(randStates), 1, rne);
		return randStates[0];
	};

	std::println("Picking a random word to generate a model from...");
	Cq::Mental::MarkovModel curModel;
	Word startingWord;
	while (true)
	{
		startingWord = getRandDictWord();
		std::println("Trying {}...", startingWord.name);
		curModel = Cq::Mental::makeMarkovModel(wordToCleanDefWords[startingWord.name], N_GRAM);
		if (curModel.size() != 0) break;
	}

	std::println("Starting model word = {}", startingWord.name);
	std::println("{}", curModel);
	StdString curState = getRandState(curModel).first;
	//	StdString nextState = NO_WORD;
	std::println("Initial state word = {}", curState);

	if (Cq::Helpers::wordCount(curState) != N_GRAM)
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
		StdStringVector possibleWords;
		const auto modelCurStateBegin = curModel[curState].cbegin();
		const auto modelCurStateEnd = curModel[curState].cend();

		possibleWords.reserve(curModel[curState].size());
		std::transform(modelCurStateBegin, modelCurStateEnd, std::back_inserter(possibleWords), [](auto tran) {
			return tran.first;
		});

		if (possibleWords.size() == 0)
		{
			curState = getRandState(curModel).first;
			++n;
			continue;
		}

		StdVector<float> probabilities;
		probabilities.reserve(possibleWords.size());
		std::transform(modelCurStateBegin, modelCurStateEnd, std::back_inserter(probabilities), [](auto tran) {
			return tran.second;
		});

		StdVector<int> indices;
		indices.reserve(possibleWords.size());

		std::discrete_distribution<> dist(probabilities.cbegin(), probabilities.cend());
		std::generate_n(std::back_inserter(indices), possibleWords.size(), [&dist, &rne]() {
			return dist(rne);
		});

		StdStringVector randWords;
		randWords.reserve(indices.size());
		std::transform(indices.cbegin(), indices.cend(), std::back_inserter(randWords), [&possibleWords](auto i) {
			return possibleWords[i];
		});

		curState = randWords[0];
		text.append(curState + " ");

		++n;
	}

	auto wc = Cq::Helpers::wordCount(text);
	std::println("\n\nGenerated text (# of words = {}): \"{}\"", wc, text);

	return 0;
}
