/* 
 * File:   httpmessage.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 2. april 2015, 10:14
 */

#include <vector>

#include "httpstuff.h"

HttpStuff::HttpStuff()
{
}

HttpStuff::~HttpStuff()
{
}

char HttpStuff::map[] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M',
	                      'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	                      'a','b','c','d','e','f','g','h','i','j','k','l','m',
	                      'n','o','p','q','r','s','t','u','v','w','x','y','z',
	                      '0','1','2','3','4','5','6','7','8','9','/','+' };

void HttpStuff::base64Encode(const std::vector<char>& bytes, std::vector<char>& encodedBytes)
{
	int bytePos = 0;
	char e1, e2, e3, e4;
	
	while (bytePos <= bytes.size() - 3)
	{
		base64Encode(e1, e2, e3, e4, bytes[bytePos], bytes[bytePos + 1], bytes[bytePos + 2]);
		encodedBytes.push_back(e1);
		encodedBytes.push_back(e2);
		encodedBytes.push_back(e3);
		encodedBytes.push_back(e4);
		bytePos += 3;
	}
	
	if (bytePos == bytes.size() - 1)
	{
		base64Encode(e1, e2, e3, e4, bytes[bytePos]);
		encodedBytes.push_back(e1);
		encodedBytes.push_back(e2);
		encodedBytes.push_back('=');
		encodedBytes.push_back('=');
	}
	else if (bytePos == bytes.size() - 2)
	{
		base64Encode(e1, e2, e3, e4, bytes[bytePos], bytes[bytePos + 1]);
		encodedBytes.push_back(e1);
		encodedBytes.push_back(e2);
		encodedBytes.push_back(e3);
		encodedBytes.push_back('=');
	}

}

void HttpStuff::base64Decode(const std::vector<char>& encodedBytes, std::vector<char>& bytes)
{
	int pos = 0;
	char b1, b2, b3;
	while (pos < encodedBytes.size() - 3)
	{
		base64Decode(b1, b2, b3, encodedBytes[pos], encodedBytes[pos + 1], encodedBytes[pos + 2], encodedBytes[pos + 3]);
		bytes.push_back(b1);
		if (encodedBytes[pos + 2] != '=') bytes.push_back(b2);
		if (encodedBytes[pos + 3] != '=') bytes.push_back(b3);
	}
}


void HttpStuff::base64Encode(char& e1, char& e2, char& e3, char& e4,
							   const char b1, const char b2, const char b3)
{
	e1 = map[(b1 & 0b11111100) >> 2];
	e2 = map[((b1 & 0b00000011) << 4) + ((b2 & 0b11110000) >> 4)];
	e3 = map[((b2 & 0b00001111) << 2) + ((b3 & 0b11000000) >> 6)];
	e4 = map[b3 & 0b00111111];
}

void HttpStuff::base64Decode(char& b1, char& b2, char& b3, const char e1, const char e2, const char e3, const char e4)
{
	char i1, i2, i3, i4;
	for (char i = 0; i < 64; i++)
	{
		if (map[i] == e1) i1 = i;
		if (map[i] == e2) i2 = i;
		if (map[i] == e3) i3 = i;
		if (map[i] == e4) i4 = i;
	}

	b1 = (i1 << 2) + ((i2 & 0b00110000) >> 4);
	if (e3 != '=')
	{
		b2 = ((i2 & 0b00001111) << 4) + ((i3 & 0b00111100) >> 2);
	}
	else 
	{
		b2 = 0;
	}

	if (e4 != '=')
	{
		b3 = ((i3 &0b00000011) << 6 + i4);
	}
	else 
	{
		b3 = 0;
	}
}
