#ifndef LIB_CQ_MENTAL_OLD
#define LIB_CQ_MENTAL_OLD

#include "helpers.h"

/*

StdMap<StdString, StdStringVector> stateMap;

StdString makePrefix(StdString w1, StdString w2)
{
	return w1 + " " + w2;
}

void insertToStateMap(StdString index, StdString value)
{
	if(stateMap.find(index) == stateMap.end()) stateMap[index] = {value};
	else stateMap[index].push_back(value);
}

StdString oldGenText()
{
	StdString w1 = NO_WORD, w2 = NO_WORD;

	std::regex wordRegex("(\\w+)");
	auto wordsBegin = std::sregex_iterator(TESTSTR.begin(), TESTSTR.end(), wordRegex);
	auto wordsEnd = std::sregex_iterator();

	for (std::sregex_iterator i = wordsBegin; i != wordsEnd; ++i)
	{
		std::smatch match = *i;
		std::string matchStr = match.str();
		trim(matchStr);
		insertToStateMap(makePrefix(w1, w2), matchStr);
		w1 = w2;
		w2 = matchStr;
	}
	insertToStateMap(makePrefix(w1, w2), NO_WORD);

	print(stateMap);
	std::random_device rd;
	std::default_random_engine randEngine(rd());

	w1 = w2 = NO_WORD;
	for(int i = 0; i < 10; ++i)
	{
		auto wordList = stateMap[makePrefix(w1, w2)];
		std::uniform_int_distribution<> randDist(0, wordList.size() - 1);
		auto ri = randDist(randEngine);
		auto nextWord = wordList[ri];

		if(nextWord == NO_WORD) break;
		print("{} ", nextWord);

		w1 = w2;
		w2 = nextWord;

	}

	return 0;
}
*/

#endif
