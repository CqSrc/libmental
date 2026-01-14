#ifndef CQ_LIB_MENTAL_WORD_H
#define CQ_LIB_MENTAL_WORD_H

#include "helpers.h"

namespace Cq
{
	namespace Mental
	{
		struct Word
		{
			StdString name;
			StdStringVector definitions;
			StdString types;
		};

		static inline bool operator<(const Word &w1, const Word &w2)
		{
			return w1.name < w2.name;
		}
	}
}

#endif