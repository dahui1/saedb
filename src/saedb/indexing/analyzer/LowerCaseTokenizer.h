#pragma once
#include "lettertokenizer.h"
class LowerCaseTokenizer :
	public LetterTokenizer
{
public:
	LowerCaseTokenizer(void)
	{
	}
	~LowerCaseTokenizer(void)
	{
	}
	LowerCaseTokenizer(string in) :LetterTokenizer(in){
    //LetterTokenizer::LetterTokenizer(in);
  }

  /** Collects only characters which satisfy
   * {@link Character#isLetter(char)}.*/
//protected:
	wchar_t normalize(wchar_t c) {
		cout<<"normalize in LowerCaseToken..."<<endl;
		return Character::toLowerCase(c);
	}
	
};

