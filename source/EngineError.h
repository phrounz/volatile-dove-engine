#ifndef EngineError_h_INCLUDED
#define EngineError_h_INCLUDED

#include "../include/Utils.h"
#include <string>

class EngineError
{
public:
	EngineError(std::wstring text) :m_text(text) { outputln(text.c_str()); }
	EngineError(std::string text) :m_text(Utils::convertStringToWString(text)) { outputln(text.c_str()); }
	std::wstring getFullText() const { return std::wstring(L"FATAL ERROR - Could not continue, please provide the message below to the developer(s) for diagnosis: \n") + m_text; }
private:
	std::wstring m_text;
};

#endif //EngineError_h_INCLUDED
