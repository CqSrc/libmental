#pragma once

#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <format>
#include <print>
#include <map>
#include <unordered_map>
#include <regex>
#include <iterator>

using StdString = std::string;

template <typename ElemType>
using StdVector = std::vector<ElemType>;

using StdStringVector = StdVector<StdString>;

template <typename FirstType, typename SecondType>
using StdPair = std::pair<FirstType, SecondType>;

template <typename KeyType, typename ValueType>
using StdMap = std::map<KeyType, ValueType>;

template <typename KeyType, typename ValueType>
using StdUnorderedMap = std::unordered_map<KeyType, ValueType>;

namespace Cq
{
	namespace Helpers
	{
		// Trim leading whitespace
		static inline void ltrim(StdString &s)
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
				return !std::isspace(ch);
			}));
		}

		// Trim trailing whitespace
		static inline void rtrim(StdString &s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
				return !std::isspace(ch);
			}).base(), s.end());
		}

		// Trim both leading and trailing whitespace
		static inline void trim(StdString &s)
		{
			ltrim(s);
			rtrim(s);
		}

		static inline StdStringVector makeCleanWords(const StdStringVector &lines)
		{
			StdStringVector cleanedWords;
			std::regex wordRegex("\\b\\w+\\b");

			for (const auto &l : lines)
			{
				StdString trimmedLine = l;
				trim(trimmedLine);
				auto wordsBegin = std::sregex_iterator(trimmedLine.begin(), trimmedLine.end(), wordRegex);
				auto wordsEnd = std::sregex_iterator();
				std::transform(wordsBegin, wordsEnd, std::back_inserter(cleanedWords), [](auto word) {
					return word.str();
				});
			}

			return cleanedWords;
		}

		static inline StdStringVector makeCleanSentences(const StdStringVector &lines)
		{
			StdStringVector cleanedSentences;
			std::regex sentenceRegex("[^.!?]*[.!?]");

			for (const auto &l : lines)
			{
				StdString trimmedLine = l;
				trim(trimmedLine);
				auto sentencesBegin = std::sregex_iterator(trimmedLine.cbegin(), trimmedLine.cend(), sentenceRegex);
				auto sentencesEnd = std::sregex_iterator();
				std::transform(sentencesBegin, sentencesEnd, std::back_inserter(cleanedSentences), [](auto sentence) {
					return sentence.str();
				});
			}

			if (cleanedSentences.size() != 0) return cleanedSentences;

			return lines;
		}

		static inline int wordCount(const StdString &str)
		{
			std::regex wordRegex("(\\w+)");
			auto wordsBegin = std::sregex_iterator(str.cbegin(), str.cend(), wordRegex);
			auto wordsEnd = std::sregex_iterator();
			return std::distance(wordsBegin, wordsEnd);
		}

		template <typename ElemType>
		static inline ElemType mostFrequentElemen(StdVector<ElemType> &arr)
		{
			int n = arr.size();
			StdUnorderedMap<ElemType, int> freq;

			for (int i = 0; i < n; i++)
				freq[arr[i]]++;

			int maxCnt = 0;
			ElemType result;

			for (const auto &[val, cnt] : freq)
			{
				if (maxCnt < cnt)
				{
					result = val;
					maxCnt = cnt;
				}
			}

			return result;
		}
	}
}

namespace Cq
{
	namespace Mental
	{
		struct Point2D
		{
			float x = 0.0f, y = 0.0f;
		};

		using Point2DVector = StdVector<Point2D>;

		static inline float euclideanDistance(Point2D p1, Point2D p2)
		{
			Point2D diff{std::pow(p1.x - p2.x, 2.0f), std::pow(p1.y - p2.y, 2.0f)};
			auto sum = diff.x + diff.y;
			return std::sqrt(sum);
		}

		template <typename LabelType>
		static inline LabelType knnPredict(const Point2DVector &trainingData, const StdVector<LabelType> &trainingLabels, Point2D testPoint, int k)
		{
			StdVector<std::pair<float, LabelType>> dists;

			for (int i = 0; i < trainingData.size(); ++i)
			{
				const auto d = euclideanDistance(testPoint, trainingData[i]);
				dists.push_back({d, trainingLabels[i]});
			}

			std::sort(std::begin(dists), std::end(dists), [](auto d1, auto d2) {
				return d1.first < d2.first;
			});

			StdVector<LabelType> knnLabels;
			knnLabels.reserve(k);

			for (int i = 0; i < k; ++i)
			{
				knnLabels.push_back(dists[i].second);
			}

			return mostFrequentElement(knnLabels);
		}
	}
/*
	static inline void test(void)
	{
		std::println("blah testing...");
	}

	template <typename T>
	void print(const T &t)
	{
		std::print("{}", t);
	}

	template <typename FirstType, typename SecondType>
	void print(const StdPair<FirstType, SecondType> &p)
	{
		print("(");
		print(p.first);
		print(", ");
		print(p.second);
		print(")");
	}

	template <typename T>
	void print(const StdVector<T> &v)
	{
		std::print("{}", v);
	}

	template <typename KeyType, typename ValueType>
	void print(const StdMap<KeyType, ValueType> &m)
	{
		std::print("{}", m);
	}

	template <typename T>
	void println(const T &t)
	{
		std::println("{}", t);
	}

	template <typename FirstType, typename SecondType>
	void println(const StdPair<FirstType, SecondType> &p)
	{
		print(p);
		print("\n");
	}

	template <typename T>
	void println(const StdVector<T> &v)
	{
		std::println("{}", v);
	}

	template <typename KeyType, typename ValueType>
	void println(const StdMap<KeyType, ValueType> &m)
	{
		std::println("{}", m);
	}

	template <typename KeyType, typename ValueType>
	void println(const StdUnorderedMap<KeyType, ValueType> &m)
	{
		std::println("{}", m);
	}
*/
}