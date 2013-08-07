#pragma once
#include "TokenStream.h"
#include "LowerCaseFilter.h"
#include "LetterTokenizer.h"
#include "StemFilter.h"
#include "StopFilter.h"
#include "ChineseTokenizer.h"

class ArnetAnalyzer
{
public:

    ArnetAnalyzer(void)
    {
    }

    ~ArnetAnalyzer(void)
    {
    }

    static TokenStream* tokenStream(const string& input)
    {
        TokenStream* le = new LetterTokenizer(input);
        le = new LowerCaseFilter(le);
        le = new StemFilter(le);
        le = new StopFilter(le);
        return le;
    }

    static TokenStream* tokenStream(const char* input, std::set<string> stopwords)
    {
        TokenStream* le = new ChineseTokenizer(input, stopwords);
        return le;
    }

    static TokenStream* tokenStream(const string& input, const int type)
    {
        TokenStream* le = new ChineseTokenizer(input);
        return le;
    }
};

