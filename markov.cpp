#include "markov.h"

#include <algorithm>
#include <numeric>

namespace Cq::Mental
{
	MarkovChain::MarkovChain(const StdStringVector &dataSet, int nGram)
	{
		modelMap = makeModel(dataSet, nGram);
	}

	MarkovModelMap MarkovChain::makeModel(const StdStringVector &textLines, int nGram)
	{
		auto cleanedLines = Helpers::makeCleanWords(Helpers::makeCleanSentences(textLines));
		auto cleanedLinesSize = cleanedLines.size();

		MarkovModelMap model;
		for (int i = 0; i < cleanedLinesSize - nGram; ++i)
		{
			StdString curState = NO_WORD, nextState = NO_WORD;
			for (int j = 0; j < nGram; ++j)
			{
				int curIndex = i + j;
				if (curIndex < cleanedLinesSize)
				{
					curState.append(cleanedLines[curIndex] + " ");
				}

				int nextIndex = i + j + nGram;
				if (nextIndex < cleanedLinesSize)
				{
					nextState.append(cleanedLines[nextIndex] + " ");
				}
			}

			Helpers::trim(curState);
			Helpers::trim(nextState);

			if (model.find(curState) == model.end())
			{
				model[curState] = {{nextState, 1.0f}};
			} else {
				if (model[curState].find(nextState) != model[curState].end())
				{
					model[curState][nextState] += 1.0f;
				} else {
					model[curState][nextState] = 1.0f;
				}
			}
		}

		for (const auto &[curState, transition] : model)
		{
			float total = std::accumulate(transition.cbegin(), transition.cend(), 0.0f, [](auto curSum, auto tran) {
				return curSum + tran.second;
			});

			for (const auto &[state, count] : transition)
			{
				model[curState][state] = count / total;
			}
		}

		return model;
	}

	void MarkovChain::reset(const StdStringVector &dataSet, int nGram)
	{
		modelMap = makeModel(dataSet, nGram);
	}

	MarkovModelStatePair MarkovChain::getRandomState(void)
	{
		MarkovModelStatePairVector randStates;
		std::sample(modelMap.cbegin(), modelMap.cend(), std::back_inserter(randStates), 1, rne);
		return randStates[0];
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

	bool MarkovChain::isEmpty(void) const
	{
		return modelMap.empty();
	}
}