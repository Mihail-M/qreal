#pragma once

#include "textLanguageParser/ast/number.h"

namespace textLanguageParser {
namespace ast {

class QRUTILS_EXPORT FloatNumber : public Number {
public:
	FloatNumber(QString const &stringRepresentation)
		: Number(stringRepresentation)
	{
	}
};

}
}