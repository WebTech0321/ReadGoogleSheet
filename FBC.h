/*
	Freelancer project:
	https://www.freelancer.com/projects/vb-net/Getting-all-coments-from-Business/proposals

	Developed by Michael H
	https://www.freelancer.com/u/NYCyber

	January 2021

*/

#pragma once

// Setings
#define DEFLOGFILENAME		L"FBC.txt"
#define COMMENTS_LIMIT		1000
#define BASIC_FILENAME		"comments-basic.json"
#define COMMENTS_FILENAME	"comments.json"
#define DEFAULT_POST_URL	"https://www.facebook.com/funerariacorreia/posts/2524125364548531"
#define USER_AGENT			"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:83.0) Gecko/20100101 Firefox/83.0"


// Constants
#define STR_TRUE			"true"
#define STR_FALSE			"false"
#define LANG				"Accept-Language: en"
#define ABC					"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define STR_POST			"POST"
#define STR_HTTP			"https"
#define STR_FEEDBACK		L"feedback"
#define STR_DATA			L"data"
#define STR_EDGES			L"edges"
#define STR_COUNT			L"count"
#define STR_PAGEINFO		L"page_info"
#define STR_HASNEXTPAGE		L"has_next_page"
#define STR_ENDCURSOR		L"end_cursor"
#define STR_NODE			L"node"
#define STR_PBODY			L"preferred_body"
#define STR_AUTHOR			L"author"
#define STR_URI				L"uri"
#define STR_ID				L"id"
#define STR_NAME			L"name"
#define STR_TEXT			L"text"
#define STR_DISPLAYCOMMENTS	L"display_comments"
#define STR_LB				L"["
#define STR_RB				L"]"

// Error messages
#define ERR_CURLEASYPERFORMFAILED	L"Curl_easy_operation() failed : %S\n"
#define ERR_ERRORFETCHINGCOMMENTS	L"\n\n======== Error fetching comments =========\n\n"
#define ERR_MUTEXCREATIONERROR		L"Mutext creation error %d\n"
#define ERR_JSONPARSING				L"Error finding PAGEINFO / other JSON parsing error"
#define ERR_IP_BLOCKED				L"Error: Your IP may have been temporarily blocked. Please wait and try again...\n"