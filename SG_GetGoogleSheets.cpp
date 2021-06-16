#include "stdafx.h"
#include "utils.h"
#include "SG_GetGoogleSheets.h"
#include "JSON.h"
#include <string>
#include <iostream>
#include <fstream>


using namespace std;

class HTMLParser
{
	std::string StringData, divText;
	int colCount, totalCols, divCount;
	bool bnfIDAdvert, bnfTHeader, bnfTRow, bnfTCol, bnSkip;
	bool bnShowHeader;

public:
	HTMLParser();
	void gotTag(string);
	void gotText(string);
	void showHeader(bool);
	string getResult();
	void ParseHTML(string html);
};

HTMLParser::HTMLParser()
{
	colCount = 0;
	totalCols = 0;
	divCount = 0;
	StringData = "";
	divText = "";
	bnfIDAdvert = false;
	bnfTHeader = false;
	bnfTRow = false;
	bnfTCol = false;
	bnSkip = false;
	bnShowHeader = true;
}

void HTMLParser::showHeader(bool bnValue)
{
	bnShowHeader = bnValue;
}

void HTMLParser::gotTag(string sTag)
{
	std::size_t found;

	if (bnfIDAdvert == false)
	{
		found = sTag.find("id='tblAdvert'");
		if (found != std::string::npos)
		{
			bnfIDAdvert = true;
		}
	}
	else
	{
		found = sTag.find("</table>");
		if (found != std::string::npos)
		{
			bnfIDAdvert = false;
		}
		else
		{
			found = sTag.find("<th");
			if (found != std::string::npos)
			{
				bnfTHeader = true;
				totalCols += 1;
			}
			found = sTag.find("<div ");
			if (found != std::string::npos)
			{
				divCount++;
			}
			found = sTag.find("</div>");
			if (found != std::string::npos)
			{
				divCount--;
			}
			found = sTag.find("<form ");
			if (found != std::string::npos)
			{
				bnSkip = true;
			}
			found = sTag.find("</form>");
			if (found != std::string::npos)
			{
				bnSkip = false;
			}
			found = sTag.find("<tr");
			if (found != std::string::npos)
			{
				bnfTRow = true;
				colCount = 0;
			}
			found = sTag.find("</tr>");
			if (found != std::string::npos && bnfTRow == true)
			{

				if (bnfTHeader == true)
				{
					if (bnShowHeader)
						StringData = StringData + "\n";
					bnfTHeader = false;
				}
				else
					StringData = StringData + "\n";
				bnfTRow = false;
			}
			found = sTag.find("<td");
			if (found != std::string::npos)
			{
				colCount = colCount + 1;
				bnfTCol = true;
			}
			found = sTag.find("</td>");
			if (found != std::string::npos && bnfTCol == true)
			{
				bnfTCol = false;
			}

		}
	}
}

void HTMLParser::gotText(string sText)
{
	if (bnfIDAdvert)
	{
		if (bnSkip) return;
		if (sText == "\r\n" || sText == "No data found") return;
		if (bnfTHeader == true)
		{
			if (bnShowHeader)
			{
				trim(sText);

				if (totalCols > 1)
					StringData = StringData + ",";
				StringData = StringData + "\"" + sText + "\"";
			}
		}
		else
		{
			if (bnfTRow && bnfTCol)
			{
				trim(sText);
				replaceAll(sText, "&nbsp;", " ");
				replaceAll(sText, "&prime;", "");
				if (divCount > 0)
				{
					divText = divText + sText + " - ";
				}
				else
				{
					if (divText != "")
					{
						StringData = StringData + ",\"" + divText + "\"";
						divText = "";
					}

					if (colCount > 1)
						StringData = StringData + ",";
					StringData = StringData + "\"" + sText + "\"";
				}
			}
		}
	}
}

string HTMLParser::getResult()
{
	return StringData;
}

void HTMLParser::ParseHTML(std::string html)
{

	for (;;)
	{
		std::string::size_type  startpos;

		startpos = html.find('<');
		if (startpos == std::string::npos)
		{
			// no tags left only text!
			gotText(html);
			break;
		}

		// handle the text before the tag    
		if (0 != startpos)
		{
			gotText(html.substr(0, startpos));
			html = html.substr(startpos, html.size() - startpos);
			startpos = 0;
		}

		//  skip all the text in the html tag
		std::string::size_type endpos;
		for (endpos = startpos;
			endpos < html.size() && html[endpos] != '>';
			++endpos)
		{
			// since '>' can appear inside of an attribute string we need
			// to make sure we process it properly.
			if (html[endpos] == '"')
			{
				endpos++;
				while (endpos < html.size() && html[endpos] != '"')
				{
					endpos++;
				}
			}
		}

		//  Handle text and end of html that has beginning of tag but not the end
		if (endpos == html.size())
		{
			html = html.substr(endpos, html.size() - endpos);
			break;
		}
		else
		{
			//  handle the entire tag
			endpos++;
			gotTag(html.substr(startpos, endpos - startpos));
			html = html.substr(endpos, html.size() - endpos);
		}
	}

}
/*

Google Sheet examples:

https://docs.google.com/spreadsheets/d/1LrlFwfK-WGeREXExJnBu7JD26fgg3US9jjh3EkyJYAM/edit?usp=sharing

*/




static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

typedef std::map<std::string, std::string> Params;


string cURL_JSON_data;
size_t writeJSONURLCallback(char * buf, size_t size, size_t nmemb, void * up) //Callback function to store the URL's data
{
	for (unsigned int c = 0; c < size * nmemb; c++)
	{
		cURL_JSON_data.push_back(buf[c]);
	}
	return size * nmemb;
}


void tokenize(vector < string > & urlComp, string & s, string del = " ")
{
	int start = 0;
	int end = s.find(del);
	while (end != -1)
	{
		urlComp.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	urlComp.push_back(s.substr(start, end - start));
}

size_t write_data_log(void *ptr, size_t size, size_t nmemb, void *userp)
{
	size_t written = size * nmemb;
	char* szTemp = new char[written + 1];
	memcpy(szTemp, ptr, written);
	szTemp[written] = NULL;
	
	printf("\n%s\n", szTemp);

	//wprintf(L"%s", szTemp);
	//written = fwrite(ptr, size, nmemb, stream);
	delete[] szTemp;
	return written;
}

bool HandleGoogleSheets(string URL)
{
	FILE * fp;
	vector < string > urlComp;

	tokenize(urlComp, URL, "/");
	string key = urlComp[5];
	string sheet = "https://sheets.googleapis.com/v4/spreadsheets/" + key + "?key=AIzaSyC9ItDX_Cqv7_31_NykJ7U9orHhzdvX3x8";
	CURL * curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_URL, sheet.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeJSONURLCallback);
		//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		struct curl_slist * headers = NULL;
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			WriteLogFile(L"Curl failed with error: %S", curl_easy_strerror(res));
		else
			WriteLogFile(L"Curl operation success");

		//Json::Reader jsonReader;
		//Json::Value jsonData;
		// cout << readBuffer << endl;

		JSONValue* value = JSON::Parse(cURL_JSON_data.c_str());
		cURL_JSON_data.clear();
		if (value != NULL)
		{
			WriteLogFile(L"Successfully parsed JSON data");
			wstring sheets = L"sheets", title = L"title", properties = L"properties";
			JSONObject root = value->AsObject();
			
			JSONArray Sheets = root[sheets]->AsArray();
			for (auto it : Sheets)
			{
				JSONObject temp = it->AsObject();
				JSONObject temp1 = temp[properties]->AsObject();
				wstring Sname = temp1[title]->AsString();
				string snew(Sname.begin(), Sname.end());
				string url = "https://docs.google.com/spreadsheets/d/" + key + "/gviz/tq?tqx=out:csv&sheet=" + snew;
				WriteLogFile(L"%S", url.c_str());
				wstring out = L"./" + Sname + L".csv";
				//char outfilename[FILENAME_MAX] = out;
				wprintf(L"%s", Sname);

				curl = curl_easy_init();
				if (curl)
				{
					//fp = _wfopen(out.c_str(), L"wb");
					curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_log);
					//curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
					res = curl_easy_perform(curl);
					if (res == CURLE_OK)
						WriteLogFile(L"Successful Request!");
					else
						WriteLogFile(L"%s", curl_easy_strerror(res));

					//JSONValue* dataValue = JSON::Parse(cURL_JSON_data.c_str());

					curl_easy_cleanup(curl);
					//fclose(fp);
				}
			}

		}
		else
		{
			WriteLogFile(L"Json parsing failed");
		}
		return true;
	}
	return false;
}
