/* 
 * File:   httpmessage.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 2. april 2015, 10:14
 */

#ifndef HTTPMESSAGE_H
#define	HTTPMESSAGE_H

class HttpStuff
{
public:
	HttpStuff();
	virtual ~HttpStuff();

private:
	static void base64Encode(const std::vector<char>& bytes, std::vector<char>& encodedBytes);
	static void base64Decode(const std::vector<char>& encodedBytes, std::vector<char>& bytes);
	static void base64Encode(char& e1, char& e2, char& e3, char& e4, const char b1, const char b2 = '\0', const char b3 = '\0');
	static void base64Decode(char& b1, char& b2, char& b3, const char e1, const char e2, const char e3, const char e4);
	static char map[];
};

#endif	/* HTTPMESSAGE_H */

