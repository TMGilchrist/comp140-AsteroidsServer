#pragma once

class Client
{
public:
	Client() { resultText = NULL; };
	~Client();

	int main(int argc, char **argv);
	void parseHighScoreJSON(void *ptr, size_t size, size_t nmemb, void *stream);
	static void staticFreeResultText();
	static void staticParseTXT(void *ptr, size_t size, size_t nmemb, void *stream);
private:
	static char* resultText;

protected:

};
