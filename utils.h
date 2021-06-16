/*
	Freelancer project:
	

	Developed by Secured Globe, Inc.
	https://www.securedglobe.net

	January 2021

*/
#pragma once
size_t write_string(void* ptr, size_t size, size_t count, void* stream);

extern wchar_t LOGFILENAME[1024];
extern HANDLE hConsole;
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int WriteLogFile(LPCWSTR lpText, ...);
string url_encode(const string& value);
bool is_base64(unsigned char c);
string base64_encode(const string& in);
string base64_decode(const string& in);
wstring string2w(const string& str);
string w2string(wstring wstr);

void replace(std::string& str, const std::string& from, const std::string& to);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
void ltrim(std::string& s);
void rtrim(std::string& s);
void trim(std::string& s);
