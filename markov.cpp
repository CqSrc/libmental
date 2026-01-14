#include "markov.h"

#include <algorithm>
#include <numeric>

namespace Cq::Mental
{
	MarkovChain::MarkovChain(const StdStringVector &dataSet, const int nGram)
	{
		modelMap = makeModel(dataSet, nGram);
		this->nGram = nGram;
	}

	MarkovModelMap MarkovChain::makeModel(const StdStringVector &textLines, const int nGram)
	{
		auto cleanedLines = Helpers::makeCleanWords(Helpers::makeCleanSentences(textLines));
		auto cleanedLinesSize = cleanedLines.size();

		MarkovModelMap model;
		for(int i = 0; i < cleanedLinesSize - nGram; ++i)
		{
			StdString curState = NO_WORD, nextState = NO_WORD;
			for(int j = 0; j < nGram; ++j)
			{
				int curIndex = i + j;
				if(curIndex < cleanedLinesSize)
				{
					curState.append(cleanedLines[curIndex] + " ");
				}

				int nextIndex = i + j + nGram;
				if(nextIndex < cleanedLinesSize)
				{
					nextState.append(cleanedLines[nextIndex] + " ");
				}
			}

			Helpers::trim(curState);
			Helpers::trim(nextState);

			if(model.find(curState) == model.end())
			{
				model[curState] = {{nextState, 1.0f}};
			} else {
				if(model[curState].find(nextState) != model[curState].end())
				{
					model[curState][nextState] += 1.0f;
				} else {
					model[curState][nextState] = 1.0f;
				}
			}
		}

		for(const auto &[curState, transition] : model)
		{
			float total = std::accumulate(transition.cbegin(), transition.cend(), 0.0f, [](auto curSum, auto tran) {
				return curSum + tran.second;
			});

			for(const auto &[state, count] : transition)
			{
				model[curState][state] = count / total;
			}
		}

		return model;
	}

	MarkovModelMap MarkovChain::makeModelFromWord(const Word &word, const int nGram)
	{
		return makeModel(word.definitions, nGram);
	}

	void MarkovChain::reset(const StdStringVector &dataSet, const int nGram)
	{
		modelMap = makeModel(dataSet, nGram);
		this->nGram = nGram;
	}

	void MarkovChain::addWordToDictionary(const Word &word)
	{
		dict.insert(word);
	}

	const MarkovModelMap &MarkovChain::getModel(void) const
	{
		return modelMap;
	}

	const Dictionary &MarkovChain::getDictionary(void) const
	{
		return dict;
	}

	WordVector MarkovChain::getRandomWordsFromDictionary(const int numWords)
	{
		WordVector randWords;
		randWords.reserve(numWords);
		std::sample(dict.cbegin(), dict.cend(), std::back_inserter(randWords), numWords, rne);
		return randWords;
	}

	MarkovModelProbMap MarkovChain::getProbMapForWord(const StdString &wordName)
	{
		if(modelMap.find(wordName) == modelMap.end()) return {};
		return modelMap.at(wordName);
	}

	StdPair<StdString, float> MarkovChain::getRandomStateForWord(const StdString &wordName)
	{
		auto probMap = getProbMapForWord(wordName);
		StdVector<StdPair<StdString, float>> randProbs;
		std::sample(probMap.cbegin(), probMap.cend(), std::back_inserter(randProbs), 1, rne);
		return randProbs[0];
	}

	MarkovModelStatePair MarkovChain::getRandomState(void)
	{
		return getRandomStates(1)[0];
	}

	MarkovModelStatePairVector MarkovChain::getRandomStates(const int numStates)
	{
		MarkovModelStatePairVector randStates;
		randStates.reserve(numStates);
		std::sample(modelMap.cbegin(), modelMap.cend(), std::back_inserter(randStates), numStates, rne);
		return randStates;
	}

	StdString MarkovChain::getPrediction(const StdString &currentState)
	{
		if(modelMap.find(currentState) == modelMap.end()) return "";

		auto modelState = modelMap.at(currentState);
		const auto modelCurStateBegin = modelState.cbegin();
		const auto modelCurStateEnd = modelState.cend();

		StdStringVector possibleWords;
		possibleWords.reserve(modelState.size());
		std::transform(modelCurStateBegin, modelCurStateEnd, std::back_inserter(possibleWords), [](const auto tran) {
			return tran.first;
		});

		if(possibleWords.size() == 0) return "";

		StdVector<float> probabilities;
		probabilities.reserve(possibleWords.size());
		std::transform(modelCurStateBegin, modelCurStateEnd, std::back_inserter(probabilities), [](const auto tran) {
			return tran.second;
		});

		StdVector<int> indices;
		indices.reserve(possibleWords.size());

		std::discrete_distribution<> dist(probabilities.cbegin(), probabilities.cend());
		std::generate_n(std::back_inserter(indices), possibleWords.size(), [&dist, this]() {
			return dist(rne);
		});

		StdStringVector randWords;
		randWords.reserve(indices.size());
		std::transform(indices.cbegin(), indices.cend(), std::back_inserter(randWords), [&possibleWords](const auto i) {
			return possibleWords[i];
		});

		return randWords[0];
	}

	void MarkovChain::setNGramSize(int nGram)
	{
		this->nGram = nGram;
	}

	void MarkovChain::setDictionary(const Dictionary &dict)
	{
		this->dict = dict;
		StdStringVector definitions;
		for(const auto &word : this->dict)
			definitions.insert(definitions.end(), word.definitions.begin(), word.definitions.end());

		reset(definitions, nGram);
	}

	bool MarkovChain::isEmpty(void) const
	{
		return modelMap.empty();
	}
}