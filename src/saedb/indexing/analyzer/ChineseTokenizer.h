#pragma once
#include <iostream>
#include <set>
#include "CharTokenizer.h"
#include "Character.h"

using namespace std;

class ChineseTokenizer :
    public TokenStream
{
public:
    ChineseTokenizer(void)
    {
    }
    ~ChineseTokenizer(void)
    {
    }

    bool isPunctuation(string s) {
        if (ispunct(s[0]) && s.size() == 1)
            return true;
        else if (s=="；"||s=="："||s=="“"||s=="”"||s=="‘"||s=="’"||s=="，"||s=="。"
            ||s=="《"||s=="》"||s=="？"||s=="、"||s=="·"||s=="！"||s=="…"||s=="（"||s=="）"
            ||s=="【"||s=="】")
            return true;
        return false;
    }

    vector<string> split(string s, char c) {
        int last = 0;
        vector<string> v;
        for (int i=0; i<s.size(); i++) {
            if (s[i] == c) {
                v.push_back(s.substr(last, i - last));
                last = i + 1;
            }
        }
        v.push_back(s.substr(last, s.size() - last));
        return v;
    }

    string filter(string s, const std::set<string>& stopwords) {
	    auto words = split(s, ' ');
        string result = "";
        for (int i = 0; i < words.size(); i++) {
            string term = words[i];
            if (!isPunctuation(term)) {
                auto stop = stopwords.find(term);
                if (stop == stopwords.end()) {
                    result += term;
                    if (i != words.size()-1) result += " ";
                }
            }
        }
        return result;
    }

    ChineseTokenizer(const char* in, const std::set<string>& stopwords) {
        offset = 0;
        const string temp = in;
        const string res = filter(temp, stopwords);
        ioBuffer = res;
        dataLen = ioBuffer.size();
    }

    ChineseTokenizer(const string& in) {
        offset = 0;
        ioBuffer = in;
        dataLen = ioBuffer.size();
    }
    
    bool next(Token& token)
    {
        int length = 0;
        int start = offset;
        wchar_t space = ' ';
        const char* cur_ptr = ioBuffer.c_str()+offset, *end_ptr = ioBuffer.c_str()+dataLen;
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
            token.setTermText(ioBuffer.c_str(),start,length);
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
    string ioBuffer;
        
};

