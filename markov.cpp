#include "markov.h"

#include <algorithm>
#include <numeric>
#include <random>

namespace Cq::Mental
{
	MarkovModel makeMarkovModel(const StdStringVector &textLines, int nGram)
	{
		auto cleanedLines = Helpers::makeCleanWords(Helpers::makeCleanSentences(textLines));
		auto cleanedLinesSize = cleanedLines.size();

		MarkovModel model;
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

	StdString makeMarkovPrediction(const MarkovModel &model, const StdString &currentState)
	{
		StdStringVector possibleWords;
		auto modelState = model.at(currentState);
		const auto modelCurStateBegin = modelState.cbegin();
		const auto modelCurStateEnd = modelState.cend();

		possibleWords.reserve(modelState.size());
		std::transform(modelCurStateBegin, modelCurStateEnd, std::back_inserter(possibleWords), [](auto tran) {
			return tran.first;
		});

		if (possibleWords.size() == 0)
		{
			return "";
			// curState = getRandState(curModel).first;
			// ++n;
		}

		StdVector<float> probabilities;
		probabilities.reserve(possibleWords.size());
		std::transform(modelCurStateBegin, modelCurStateEnd, std::back_inserter(probabilities), [](auto tran) {
			return tran.second;
		});

		StdVector<int> indices;
		indices.reserve(possibleWords.size());

		std::random_device rd;
		std::default_random_engine rne(rd());
		std::discrete_distribution<> dist(probabilities.cbegin(), probabilities.cend());
		std::generate_n(std::back_inserter(indices), possibleWords.size(), [&dist, &rne]() {
			return dist(rne);
		});

		StdStringVector randWords;
		randWords.reserve(indices.size());
		std::transform(indices.cbegin(), indices.cend(), std::back_inserter(randWords), [&possibleWords](auto i) {
			return possibleWords[i];
		});

		return randWords[0];
	}
}