#include <locale>
#include <iostream>

#include "analyzer.hpp"
#include "analyzer/utils.hpp"

using namespace std;

int main() {
    string str = "hehe good good study 今天 天气 好晴朗";
    const char* test = str.c_str();
    std::unique_ptr<TokenStream> stream(ArnetAnalyzer::tokenStream(test));
    Token token;
    while (stream->next(token)) {
        string term = token.getTermText();
        cout << term << endl;
    }
    set_conversion_locale(locale("en_US.UTF-8"));
    wstring s = L"hehe 天气不错哦哦哦哦";
    cout << s.size() << "===" << wstring_to_string(s) << endl;
    string temp = "天气不错呦";
    wstring sub;
    sub.assign(s, 0, 8);
    string result = wstring_to_string(s);
    for (int i = 0; i < sub.size(); i++)
        cout << sub[i] << endl;
    cout << result << " " << result.size() << endl;
}
