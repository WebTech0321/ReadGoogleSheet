/*
	Freelancer project:
	https://www.freelancer.com/projects/vb-net/Getting-all-coments-from-Business/proposals

	Developed by Michael H
	https://www.freelancer.com/u/NYCyber

	January 2021

*/
#include "stdafx.h"
#include "utils.h"
#include "FBC.h"

HANDLE hConsole;

JSONObject fetch_more_comments(string post_id, int comment_limit, string last_comment_id)
{
	// Initialize request parameters
	char *str7 = s_3399792706();
	char *str8 = s_2842369719();
	char *str9 = s_2842369720();
	string url = s_406175739();		;
	string feedback_id = str9 + post_id;
	feedback_id = base64_encode(feedback_id);
	feedback_id = url_encode(feedback_id);
	string after = empty(last_comment_id) ? "" : str7 + url_encode(last_comment_id) + str8;
	string is_paginating = STR_TRUE; 
	string is_initial_fetch = STR_FALSE; 

   // Build curl request
	CURL* curl;
	CURLcode res;
	string readBuffer;
	curl = curl_easy_init();
	RandomSleep(2000);
	// Obfuscated data
	char *str1 = s_2826945120();
	char *str2 = s_350095756();
	char *str3 = s_3548910080();
	char *str4 = s_989411961();
	char *str5 = s_1882776466();
	char *str6 = s_1508892970();
	// Start Crawling...
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, STR_POST);
		curl_easy_setopt(curl, CURLOPT_URL, s_2021409656());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, STR_HTTP);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, USER_AGENT);
		headers = curl_slist_append(headers, s_980717551());
		headers = curl_slist_append(headers, LANG);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		RandomSleep(2000);
		const string data = str1 + after + str2 + feedback_id + str3 + to_string(comment_limit) + str4 + is_initial_fetch + str5 + is_paginating + str6;
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		RandomSleep(2000);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			WriteLogFile(ERR_CURLEASYPERFORMFAILED, curl_easy_strerror(res));
		}
	}
	curl_easy_cleanup(curl);
	if (!empty(readBuffer))
	{
		WriteLogFile(L"Read buffer not empty\n");
		JSONValue* jsonValue = JSON::Parse(readBuffer.c_str());
		WriteLogFile(L"Parsed %S\njsonValue = %S",
			readBuffer.c_str(),
			jsonValue->AsString().c_str());
		WriteLogFile(L"Getting root");
		JSONObject root = jsonValue->AsObject();
		if (root.find(STR_DATA) == root.end())
		{
			WriteLogFile(ERR_IP_BLOCKED);
			return root;
		}
		else
		{
			WriteLogFile(L"Found STR_DATA");
			JSONObject data = root[STR_DATA]->AsObject();

			WriteLogFile(L"jsonValue (2) = %S",
				root[STR_DATA]->AsString().c_str());

			if (data.find(STR_FEEDBACK) == data.end())
			{
				WriteLogFile(ERR_IP_BLOCKED);
				return root;
			}
			if (data[STR_FEEDBACK]->IsObject() == NULL)
			{
				WriteLogFile(ERR_IP_BLOCKED);
				return root;

			}
			JSONObject feedback = data[STR_FEEDBACK]->AsObject();
			WriteLogFile(L"Found STR_FEEDBACK");

			if (feedback.find(STR_DISPLAYCOMMENTS) == feedback.end())
			{
				WriteLogFile(ERR_IP_BLOCKED);
				return root;
			}
			WriteLogFile(L"Found STR_DISPLAYCOMMENTS");

			JSONObject display_comments = feedback[STR_DISPLAYCOMMENTS]->AsObject();
			return display_comments;
		}
	}
	WriteLogFile(L"Read buffer is empty\n");
	return JSONObject();
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	LPWSTR *szArglist{ NULL };
	int nArgs;
	int i;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (szArglist == NULL)
	{
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}
	else 
	for (i = 0; i < nArgs; i++) 
		wprintf(L"%d: %ws\n", i, szArglist[i]);


	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL, szExeFileName, MAX_PATH);
	PathStripPath(szExeFileName);

	string post_url{ DEFAULT_POST_URL };
	string outputfile{ COMMENTS_FILENAME };
	wcscpy(LOGFILENAME, DEFLOGFILENAME);

	if (nArgs < 4)
	{
#ifdef _DEBUG
		WriteLogFile(L"USAGE: %s <Facebook Post URL> <JSON File> <Log File>\n", szExeFileName);
#else
		MessageBox(GetActiveWindow(), L"USAGE: %s <Facebook Post URL> <JSON File> <Log File>", L"FBC", MB_OK);
		exit(-1);
#endif
	}
	else
	{
		wstring arg1(szArglist[1]);
		wstring arg2(szArglist[2]);
		post_url = w2string(arg1);
		outputfile = w2string(arg2);
		wcscpy(LOGFILENAME, szArglist[3]);
	}
	if (post_url[post_url.size() - 1] == '/')
	{
		post_url= post_url.substr(0, post_url.size() - 1);
	}
	LocalFree(szArglist);

	HANDLE hMutex;
	hMutex = CreateMutex(
		NULL,					// default security descriptor
		FALSE,                  // mutex not owned
		L"FBC");				// object name

	if (hMutex == NULL)
		WriteLogFile(ERR_MUTEXCREATIONERROR, GetLastError());
	else
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		WriteLogFile(L"Software already running\n");
		exit(0);
	}


	WriteLogFile(L"Welcome to %s\n%d args were sent\n\n", szExeFileName, nArgs - 1);
	// Initialize facebook post url
	WriteLogFile(L"URL:\t%S\n", post_url.c_str());
	WriteLogFile(L"Output File:\t%S\n", outputfile.c_str());

	// Get post id from url
	string post_id = post_url.substr(post_url.find_last_of("/") + 1);
	string::size_type const p(post_id.find_last_of('?'));
	post_id = post_id.substr(0, p);
	WriteLogFile(L"Post ID:\t%S\n", post_id.c_str());
	int comment_limit = COMMENTS_LIMIT;
	try
	{
	   // Fetch initial comments
		wstring last_comment_id = L"";
		JSONObject comments = fetch_more_comments(post_id, comment_limit, w2string(last_comment_id));
		vector<JSONValue> comments_arr;
		int comment_count = 0;
		if (!comments.empty() && (comments.find(STR_EDGES) != comments.end()))
		{
			JSONArray edges = comments[STR_EDGES]->AsArray();
			comment_count = edges.size();
			int comments_count_total = (int)comments[STR_COUNT]->AsNumber();
			JSONObject page_info = comments[STR_PAGEINFO]->AsObject();
			bool has_next_page = page_info[STR_HASNEXTPAGE]->AsBool();
			last_comment_id = page_info[STR_ENDCURSOR]->AsString();

			// Add comment page to array
			// A single comment page contains upto $comment_limit of user comments
			// Comment pages are just paginated responses
			comments_arr.push_back(JSONValue(comments));

			while (has_next_page && comment_count < comment_limit)
			{
				WriteLogFile(L"Reading comment %d\n", comment_count);
			// Fetch more comments
				JSONObject comments = fetch_more_comments(post_id, comment_limit, w2string(last_comment_id));
				if (comments.find(STR_PAGEINFO) == comments.end())
				{
					WriteLogFile(ERR_JSONPARSING);
					break;
				}
				
				JSONObject page_info = comments[STR_PAGEINFO]->AsObject();
				// Get next page info
				has_next_page = page_info[STR_HASNEXTPAGE]->AsBool();
				last_comment_id = page_info[STR_ENDCURSOR]->AsString();
				RandomSleep(2000);
				// Add the new comment page to array
				JSONValue icomments = JSONValue(comments);
				comments_arr.push_back(icomments);
				edges = comments[STR_EDGES]->AsArray();
				comment_count += edges.size();
			}
		}

		WriteLogFile(L"\n======== Comment fetch ended =========\n");
		WriteLogFile(L"\n======== Total comments retrieved (%d)=========\n", comment_count);

		if (comment_count > 0)
		{
			// At this point we have retrieved all comments
			// Next is to format the response and save to file
			wstring commentsW = STR_LB;
			for (unsigned int i = 0; i < comments_arr.size(); i++)
			{
				JSONValue* value = &comments_arr.at(i);
				commentsW += value->Stringify();
				if (i + 1 < comments_arr.size())commentsW += L",";
			}
			commentsW += STR_RB;
			// Format json
			JSONValue* jsonValue = JSON::Parse(commentsW.c_str());
			string commentsJson = w2string(jsonValue->Stringify(true));
			//WriteLogFile(L"%s\n",commentsJson);
			// Save response to file
			ofstream file(outputfile);
			file << commentsJson;
			file.close();
			WriteLogFile(L"\n\n======== Response saved to file  =========\n\n");



		//... We can go on further to filter only the needed attributes
		// Building basic comment list from response (See comments.json for all attributes)
			wstring comments_basic = STR_LB;
			WriteLogFile(L"\n\n======== Basic Comment info started =========\n\n");
			int appended = 0;
			for (JSONValue _comments : comments_arr)
			{
				JSONValue* page = &_comments;
				JSONObject _page = page->AsObject();
				JSONArray edges = _page[STR_EDGES]->AsArray();
				for (JSONValue* edge : edges)
				{
					JSONObject _edge = edge->AsObject();
					JSONObject node = _edge[STR_NODE]->AsObject();
					bool comment_available = node[STR_PBODY]->IsObject();
					JSONObject comment_body = comment_available ? node[STR_PBODY]->AsObject() : JSONObject();
					JSONObject author = node[STR_AUTHOR]->AsObject();
					JSONObject author_profile_pic = author[s_3304722366()]->AsObject();
					// Basic info
					wstring author_photo = author_profile_pic[STR_URI]->AsString();
					wstring author_id = author[STR_ID]->AsString();
					wstring author_name = author[STR_NAME]->AsString();
					wstring author_comment = comment_available ? comment_body[STR_TEXT]->AsString() : L"";


					WriteLogFile(L"Comment:\t\t '%s'", author_comment.c_str());
					WriteLogFile(L"Posted by:\t\t '%s'", author_id.c_str());
					int comment_date = (int)node[L"created_time"]->AsNumber();
					string icomment = w2string(author_comment);
					icomment = std::regex_replace(icomment, std::regex("\""), "\\\"");
					icomment = std::regex_replace(icomment, std::regex("\n"), " ");
					author_comment = string2w(icomment);
					comments_basic += L"{";
					comments_basic += L"\"author_id\": \"" + author_id + L"\",";
					comments_basic += L"\"author_name\": \"" + author_name + L"\",";
					comments_basic += L"\"author_photo\": \"" + author_photo + L"\",";
					comments_basic += L"\"author_comment\": \"" + author_comment + L"\",";
					comments_basic += L"\"comment_date\":" + to_wstring(comment_date);
					comments_basic += L"}";
					appended++;
					if (appended < (int)edges.size() * (int)comments_arr.size()) comments_basic += L",";
				}
			}
			comments_basic += STR_RB;
			// Format json
			JSONValue* jsonValue2 = JSON::Parse(comments_basic.c_str());
			if (jsonValue2)
			{
				commentsJson = w2string(jsonValue2->Stringify(true));
				//WriteLogFile(L"%S\n", commentsJson.c_str());
				// Save basic comments to file
				ofstream file2(BASIC_FILENAME);
				file2 << commentsJson;
				file2.close();
				WriteLogFile(L"\n======== Basic Comment info ended =========\n");
				WriteLogFile(L"\n\n======== Basic comments saved to file (%S) =========\n\n",BASIC_FILENAME);
			}
		}
	}
	catch (exception ex)
	{
		WriteLogFile(ERR_ERRORFETCHINGCOMMENTS);
	}
#ifdef _DEBUG
	system("pause");
#endif
	return 0;
}