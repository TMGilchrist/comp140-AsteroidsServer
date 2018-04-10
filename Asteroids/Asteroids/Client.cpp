#include "stdafx.h"
#include "Client.h"

#include "../libcurl/include/curl/curl.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../rapidjson/rapidjson.h"

#pragma comment(lib, "../libcurl/lib/libcurl_a.lib")

#include "../rapidjson/document.h"
using namespace rapidjson;

char* Client::resultText;

Client::~Client()
{
}

int Client::main(int argc, char **argv)
{
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);

	//Add JSON :)	
	//https://www.codeproject.com/Articles/1102603/Accessing-JSON-Data-with-Cplusplus

	/* get a curl handle */


	bool bQuit = false;
	int score = 1;

	while (bQuit == false)
	{
		printf("1-POST\n");
		printf("2-GET\n");
		printf("X-QUIT\n");

		char ch = getchar();
		fseek(stdin, 0, SEEK_END);

		printf("\n\n");

		if (ch == '1')		//POST
		{
			curl = curl_easy_init();
			curl_slist* headers = NULL;

			//headers = curl_slist_append(headers, "client_id_header");
			headers = curl_slist_append(headers, "application/x-www-form-urlencoded");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:80/post_newscore/");
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST"); /* !!! */

			char text[255] = "";

			sprintf(text, "insert into highscores(name, score) values(\"Me\", %d)", score++);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, text); /* data goes here */
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, staticParseTXT);

			res = curl_easy_perform(curl);

			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}

		if (ch == '2') //GET
		{
			curl = curl_easy_init();
			curl_slist* headers = NULL;

			//headers = curl_slist_append(headers, "client_id_header");
			headers = curl_slist_append(headers, "application/x-www-form-urlencoded");

			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:80/get_highscores/");
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); /* !!! */

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ""); /* data goes here */
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, staticParseTXT);

			staticFreeResultText();

			res = curl_easy_perform(curl);

			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);

			if (resultText != NULL)
			{
				char str[1024] = "";

				sprintf(str, "{\"scores\": %s}", resultText);

				Document document;
				document.Parse(str);

				for (SizeType i = 0; i < document["scores"].Size(); i++)
				{
					printf("{Name: %s, Score=%s}\n", document["scores"][i]["name"].GetString(), document["scores"][i]["score"].GetString());
				}
			}
		}

		if ((ch == 'X') || (ch == 'x'))
		{
			bQuit = true;
		}
	}


	curl_global_cleanup();
	return 0;
}

void Client::parseHighScoreJSON(void *ptr, size_t size, size_t nmemb, void *stream)
{
	printf((char*)ptr);

	Document document;
	document.Parse((char*)ptr);

	for (SizeType i = 0; i < document["results"].Size(); i++)
	{
		printf("{Name: %s, Score=%s}\n", document["results"][i]["name"].GetString(), document["results"][i]["score"].GetString());
	}


	printf("");
}

void Client::staticFreeResultText()
{
	if (resultText != NULL)
	{
		delete[] resultText;
	}

	resultText = NULL;
}

void Client::staticParseTXT(void * ptr, size_t size, size_t nmemb, void * stream)
{
	staticFreeResultText();

	resultText = new char[nmemb + 1];
	memset(resultText, 0, nmemb + 1);
	memcpy(resultText, ptr, nmemb);
}
