#ifndef EngineError_h_INCLUDED
#define EngineError_h_INCLUDED

#include "../include/Utils.h"
#include <string>

class EngineError
{
public:
	EngineError(std::string text) :m_text(text) { outputln(text.c_str()); }
	std::string getFullText() const { return std::string("FATAL ERROR - Could not continue, please provide the message below to the developer(s) for diagnosis: \n") + m_text; }
private:
	std::string m_text;
};

#endif //EngineError_h_INCLUDED
