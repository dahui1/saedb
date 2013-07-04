#include "StemFilter.h"

StemFilter::StemFilter(TokenStream* in)
	:TokenFilter(in)
{

}

bool StemFilter::next(Token& token){
	bool more = input->next(token);
	if (!more) return false;

	wstring str = token.getWsTermText();
	wstring originalStr = str;
	StemEnglish(str);
	token.setTermText(str);
	return true;
}
