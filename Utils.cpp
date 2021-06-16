/*
	Freelancer project:


	Developed by Secured Globe, Inc.
	https://www.securedglobe.net

	January 2021

*/
#include "stdafx.h"
#include "GetGoogleSheets.h"
#include "utils.h"
wchar_t LOGFILENAME[1024]{ L"Reposter_Log.txt" };

size_t write_string(void* ptr, size_t size, size_t count, void* stream)
{
	((string*)stream)->append((char*)ptr, 0, size * count);
	return size * count;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

// Function:	WriteLogFile
// Purpose:		Print a message to the log file and (in DEBUG) to the Console
int WriteLogFile(LPCWSTR lpText, ...)
{
	FILE *fp;
	CTime Today = CTime::GetCurrentTime();

	CAtlString sMsg;
	CAtlString sLine;
	va_list ptr;
	errno_t errCode;
	va_start(ptr, lpText);
	sMsg.FormatV(lpText, ptr);

	sLine.Format(L"%s: %s\n", (LPCTSTR)Today.FormatGmt(L"%d.%m.%Y %H:%M"), (LPCTSTR)sMsg);
	CStringA line = (CStringA)sMsg + "\n";
	try
	{
		errCode = _wfopen_s(&fp, LOGFILENAME, L"a,ccs=UTF-8");
		if (fp && (errCode == 0))
		{
			fwprintf(fp, L"%s\n", sLine.GetBuffer());

			// Added by BOZO
			sLine.ReleaseBuffer();

			if (fp->_Placeholder)
				fclose(fp);
		}
#ifdef _DEBUG
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#else
		wprintf(L"%S\n", line.GetBuffer());
#endif
		if (hConsole)
		{
			WriteFile(
				hConsole,               // output handle 
				line.GetBuffer(),       // prompt string 
				line.GetLength(),		// string length 
				NULL,					// bytes written 
				NULL);
		}
	}
	catch (...)
	{
		if (fp->_Placeholder)
			fclose(fp);

		return FALSE;
	}

	return TRUE;
}


// Function:	url_encode
// Purpose:		encode a given URL
string url_encode(const string& value)
{
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
	{
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		escaped << nouppercase;
	}

	return escaped.str();
}

// Function:	is_base64
// Purpose:		Returns a Boolean result based on whether a given char is base 64
bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

// Function:	base64_encode
// Purpose:		Base64 encode a given string (ASCII)
string base64_encode(const string& in)
{

	string out;

	int val = 0, valb = -6;
	for (char c : in)
	{
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0)
		{
			out.push_back(ABC[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back(ABC[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}
// Function:	base64_encode
// Purpose:		Base64 decode an encoded given string (ASCII)
string base64_decode(const string& in)
{

	string out;

	vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T[ABC[i]] = i;

	int val = 0, valb = -8;
	for (char c : in)
	{
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0)
		{
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

// Function:	string2w
// Purpose:		convert a string to wstring
wstring string2w(const string& str)
{
	if (str.empty())
		return wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
	{
		WriteLogFile(L"Failed converting UTF-8 string to UTF-16");
		return L"";
	}

	vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
	{
		WriteLogFile(L"Failed converting UTF-8 string to UTF-16");
		return L"";
	}
	return wstring(&buffer[0], charsConverted);
}

// Function:	string2w
// Purpose:		convert a wstring to string
string w2string(wstring wstr)
{
	if (wstr.empty())
	{
		return string();
	}
#if defined WIN32
	int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], wstr.size(), NULL, 0, NULL, NULL);
	string ret = string(size, 0);
	WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, &wstr[0], wstr.size(), &ret[0], size, NULL, NULL);
#else
	size_t size = 0;
	_locale_t lc = _create_locale(LC_ALL, "en_US.UTF-8");
	errno_t err = _wcstombs_s_l(&size, NULL, 0, &wstr[0], _TRUNCATE, lc);
	string ret = string(size, 0);
	err = _wcstombs_s_l(&size, &ret[0], size, &wstr[0], _TRUNCATE, lc);
	_free_locale(lc);
	ret.resize(size - 1);
#endif
	return ret;
}

void replace(std::string& str, const std::string& from, const std::string& to)
{
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return;
	str.replace(start_pos, from.length(), to);
	return;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}
// trim from start (in place)
void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
	{
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
	{
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
void trim(std::string& s)
{
	ltrim(s);
	rtrim(s);
}
