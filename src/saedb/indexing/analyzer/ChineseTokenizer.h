#pragma once
#include <iostream>
#include "CharTokenizer.h"
#include "Character.h"

using std::string;

class ChineseTokenizer :
	public Tokenizer
{
public:
	ChineseTokenizer(void)
	{
	}
	~ChineseTokenizer(void)
	{
	}
	ChineseTokenizer(const string& in) {
		offset = 0;
		dataLen = input.length();
		ioBuffer = TokenUtil::s2wchar(input);
	}
	
	bool next(Token& token)
	{
		int length = 0;
		int start = offset;
		wchar_t* cur_ptr = ioBuffer+offset, *end_ptr = ioBuffer+dataLen;
		while (cur_ptr!=end_ptr)
		{
			++offset;
			if (*cur_ptr != ' ')
			{
				++length;
			}
			else
			{
				if (length) break;
				start = offset;
			}
			++cur_ptr;
		}

		if (length)
		{
			//wstring wstr(ioBuffer+start, ioBuffer+start+length);
			//token.setTermText(TokenUtil::ws2s(wstr));
			token.setTermText(ioBuffer,start,length);
			token.setStartOffset(start);
			token.setEndOffset(start+length);
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	int offset, dataLen;
	wchar_t* ioBuffer;
};

