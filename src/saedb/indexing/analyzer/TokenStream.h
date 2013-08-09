#pragma once
#include <iostream>
#include "Token.h"

using std::string;

class TokenStream
{
public:
	TokenStream(void)
	{
	}
	virtual ~TokenStream(void)
	{
	}

	//Token next();
	/*virtual Token next(){
		Token t; 
		return t;
	};*/
	virtual bool next(Token & token){
		return true;
	}
	//virtual bool hasMore(){return false;};
	//virtual bool hasMore(){return true;};
	//bool hasMore();
	
	/*virtual bool hasMore(){
		cout<<"hasMore in TokenStream.h"<<endl;
		return false;
	};*/
	/** Resets this stream to the beginning. This is an
   *  optional operation, so subclasses may or may not
   *  implement this method. Reset() is not needed for
   *  the standard indexing process. However, if the Tokens 
   *  of a TokenStream are intended to be consumed more than 
   *  once, it is neccessary to implement reset(). 
   */
	//virtual void reset(){}; 
	virtual void reset(){}; 
};

