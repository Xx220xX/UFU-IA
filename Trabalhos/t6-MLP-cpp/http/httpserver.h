//
// Created by hslhe on 16/09/2023.
//

#ifndef CREST_HTTPSERVER_H
#define CREST_HTTPSERVER_H

#include <string>
#include <vector>
#include "json.h"

enum ContentType {
    CT_HTML, CT_IM_ICON, CT_IM_PNG, CT_IM_JPEG, CT_CSS, CT_JS, CT_JSON, CT_XML, CT_Text, CT_Image, CT_UNKNOW
};
#define HTTP_POST 0x01
#define HTTP_GET 0x02
#define HTTP_PUT 0x03
#define HTTP_DELETE 0x04
#define HTTP_UPDATE 0x05

class HTTPServer;

typedef std::string (*Method)(HTTPServer &, json::JSON &);

#define JWT_KEY "dasofbewifbq4b";
#define JWT_EXPIRE 5000

class HTTPServer {
private:
    long long int serverSocket = -1; // Socket para a conexão HTTP
    long long int serverSocket6 = -1; // Socket para a conexão HTTP
    std::string html_not_found;

//	std::vector<std::string> nomesRotas;

public:

    std::map<std::string, Method> rotas;
    bool run = true;
    void *info = nullptr;

    HTTPServer();


    static std::string jwthex(std::string &hexdata, std::string &ip);

    static std::string jwt(json::JSON &dados, std::string &ip);

    static std::string getToken(std::string user, std::string &client_ip);

    bool authenticate(json::JSON &request);


    void ipv4INIT(const char *ip, int port);

    void ipv6INIT(const char *ip, int port);

    int startListening();

    void acceptConnections(long long int server);

    void processHttpRequest(int client, json::JSON &request);

    std::string renderTextResponse(const char *html_file, ContentType type = CT_HTML, int htmlError = 200);

    std::string renderBinaryResponse(const char *file, ContentType type, int htmlError);

    std::string redirect(const char *url, int htmlError);

    std::string Response(int code, ContentType type, std::string &response);

    std::string Response(int code, json::JSON &response);

    std::string Response(int code, ContentType type, const char *response);
    std::string NOTFOUND();

    void Rota(const char *method, const char *string, Method pFunction);

};

void beginHTTP();

void endHTTP();

#define HTTP() [](HTTPServer &self, json::JSON &request) -> std::string
#define NAUTH(notauthenticate)if(!server.authenticate(request)){notauthenticate}
#define ROUTE(srv, METHOD, ROTA, x)srv.Rota(METHOD,ROTA,HTTP(){x})
#endif //CREST_HTTPSERVER_H
