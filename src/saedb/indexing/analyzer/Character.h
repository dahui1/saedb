#pragma once
#include<string>
using std::string;
using std::wstring;

class Character
{
public:
	Character(void)
	{
	}
	~Character(void)
	{
		//do nothing
	}
	/*
	* Variables
	*/
	
    //static int MAX_CODE_POINT; //The maximum value of a Unicode code point.

	bool isLetter(wchar_t codePoint) {
        bool bLetter = false;

        if (!(codePoint>>8)) {
            bLetter = CharacterDataLatin1_isLetter(codePoint);
        } else {
            int plane = getPlane(codePoint);
            switch(plane) {
            case(0):
                //bLetter = CharacterData00.isLetter(codePoint);
				bLetter = true; //TODO...
                break;
            case(1):
                //bLetter = CharacterData01.isLetter(codePoint);
				bLetter = true; //TODO...
                break;
            case(2):
                //bLetter = CharacterData02.isLetter(codePoint);
				bLetter = true; //TODO...
                break;
            case(3): // Undefined
            case(4): // Undefined
            case(5): // Undefined
            case(6): // Undefined
            case(7): // Undefined
            case(8): // Undefined
            case(9): // Undefined
            case(10): // Undefined
            case(11): // Undefined
            case(12): // Undefined
            case(13): // Undefined
                //bLetter = CharacterDataUndefined.isLetter(codePoint);
				bLetter = true; //TODO...
                break;
            case(14):
                //bLetter = CharacterData0E.isLetter(codePoint);
				bLetter = true; //TODO...
                break;
            case(15): // Private Use
            case(16): // Private Use
                //bLetter = CharacterDataPrivateUse.isLetter(codePoint);
				bLetter = false;
                break;
            default:
                // the argument's plane is invalid, and thus is an invalid codepoint
                // bLetter remains false;
                break;
            }
        }
        return bLetter;
    }
	static wchar_t toLowerCase(wchar_t ch) {
		if(ch>='A' && ch<='Z')
			return (wchar_t)(ch-'A'+'a');
		else
			return ch;
    }
	 
	static char toLowerCase(char ch) {
		if(ch>='A' && ch<='Z')
			return (char)(ch-'A'+'a');
		else
			return ch;
    }

	static void toLowerCase(wstring& str) {
		int len = str.length();
		for (int i=0;i<len;++i)
		{
			str[i] = toLowerCase(str[i]);
		}
	}

	static void toLowerCase(string& str) {
		int len = str.length();
		for(int i=0;i<len;i++){
			str[i] = toLowerCase(str[i]);
		}
    }
	/*
	 * Utils for Character.isLetter...
	 */
	bool CharacterDataLatin1_isLetter(wchar_t ch) {
        bool isLetter = false;
		if(ch>=97 && ch<=122) //A-Z
			isLetter = true;
		else if(ch>=65 && ch<=90) //a-z
			isLetter = true;
		else if(ch == 170 || ch == 181 || ch == 186)
			isLetter = true;
		else if( (ch>=192 && ch<=214) || (ch>=216 && ch<=246) || (ch>=248 && ch<=255)) 
			isLetter = true;
		return isLetter;
    }
	int getPlane(int ch) {
        return (ch >> 16);
    }
};

