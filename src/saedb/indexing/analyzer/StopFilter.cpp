#include "StopFilter.h"

unordered_set<wstring> StopFilter::stopTokens = {
	L"a", L"an", L"and", L"are", L"as", L"at", L"be", L"but", L"by", L"for",
	L"if", L"in", L"into", L"is", L"it", L"no", L"not", L"of", L"on", L"or",
	L"such", L"that", L"the", L"their", L"then", L"there", L"these", L"they", L"this",
	L"to", L"was", L"will", L"with"
};

StopFilter::StopFilter(TokenStream* in) : TokenFilter(in) { }

bool StopFilter::next(Token& token) {
	bool more = input->next(token);
	while (more)
	{
		const wstring& str = token.getWsTermText();
		if (stopTokens.find(str) == stopTokens.end())
		{
			return true;
		}
		more = input->next(token);
	}
	return false;
}
