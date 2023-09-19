//
// Created by hslhe on 16/09/2023.
//

#ifndef CREST_HTTPSERVER_H
#define CREST_HTTPSERVER_H

#include <string>
#include <vector>
#include "json.h"

enum ContentType {
	HTML, JSON, XML, Text, Image, UNKNOW_TYPE
};
#define HTTP_POST 0x01
#define HTTP_GET 0x02
#define HTTP_PUT 0x03
#define HTTP_DELETE 0x04
#define HTTP_UPDATE 0x05

class HTTPServer;

typedef std::string (*Method)(HTTPServer &, json::JSON &);


class HTTPServer {
private:
	long long int serverSocket = -1; // Socket para a conexão HTTP
	long long int serverSocket6 = -1; // Socket para a conexão HTTP
	std::string html_not_fount;
//	std::vector<std::string> nomesRotas;

public:
	std::map<std::string, Method> rotas;
	void *info = nullptr;

	HTTPServer() {
		html_not_fount = "<!DOCTYPE html>\n"
						 "<html lang=\"en\">\n"
						 "<head>\n"
						 "    <meta charset=\"UTF-8\">\n"
						 "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
						 "    <title>404 - Not Found</title>\n"
						 "    <style>\n"
						 "        body {\n"
						 "            font-family: Arial, sans-serif;\n"
						 "            text-align: center;\n"
						 "            background-color: #f4f4f4;\n"
						 "        }\n"
						 "        .container {\n"
						 "            padding: 50px;\n"
						 "        }\n"
						 "        h1 {\n"
						 "            font-size: 3em;\n"
						 "            color: #ff6347;\n"
						 "        }\n"
						 "        p {\n"
						 "            font-size: 1.5em;\n"
						 "            color: #333;\n"
						 "        }\n"
						 "    </style>\n"
						 "</head>\n"
						 "<body>\n"
						 "    <div class=\"container\">\n"
						 "        <h1>404 - Not Found</h1>\n"
						 "        <p>The page you are looking for does not exist.</p>\n"
						 "    </div>\n"
						 "</body>\n"
						 "</html>";
	};

	void ipv4INIT(const char *ip, int port);

	void startListening();

	void acceptConnections(long long int server);

	void processHttpRequest(int client, json::JSON &request);

	std::string renderHtmlResponse(const char *html_file, int htmlError = 200);

	std::string Response(int code, ContentType type, std::string &response);

	std::string Response(int code, json::JSON &response);

	std::string Response(int code, ContentType type, const char *response);

	void Rota(const char *method, const char *string, Method pFunction);

	void Path(const char *routePrefix, const char *path);
};

#define HTTP() [](HTTPServer &self, json::JSON &b) -> std::string

#endif //CREST_HTTPSERVER_H
