//
// Created by hslhe on 16/09/2023.
//

#include "httpserver.h"
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <exception>

//#define  SOCKET_NAO_BLOQUEANTE
#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
#define CLOSE_SOCKET(sock) closesocket(sock)
#define SOCKET_READ(sock, buff, len)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define CLOSE_SOCKET(sock) close(sock)
#endif
const char corsHeaders[] = "Access-Control-Allow-Origin: *\r\n"
						   "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
						   "Access-Control-Allow-Headers: Content-Type\r\n";

void HTTPServer::ipv4INIT(const char *ip, int port) {
	struct sockaddr_in serverAddr = {0};
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		perror("Erro ao inicializar o Winsock");
		exit(EXIT_FAILURE);
	}
#endif
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("Erro ao criar socket IPv4");
		exit(EXIT_FAILURE);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &serverAddr.sin_addr);

	if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
		perror("Erro ao vincular socket IPv4");
		exit(EXIT_FAILURE);
	}


	if (listen(serverSocket, 10) == -1) {
		perror("Erro ao ouvir por conexões IPv4");
		exit(EXIT_FAILURE);
	}

	printf("Servidor IPv4 %s%s:%d ...\n", isdigit(ip[0]) ? "http://" : "", ip, port);
}

void HTTPServer::startListening() {
	if (this->serverSocket == -1 && this->serverSocket6 == -1) {
		fprintf(stderr, "No server up\n");
		return;
	}
	for (;;) {
		fflush(stdout);
		std::cout << "Aguardando requisições\n";
		fd_set fds;
		FD_ZERO(&fds);
		if (serverSocket != -1)
			FD_SET(serverSocket, &fds);
		if (serverSocket6 != -1)
			FD_SET(serverSocket6, &fds);

		long long int maxSocket = (serverSocket > serverSocket6) ? serverSocket : serverSocket6;
		if (select((int) maxSocket + 1, &fds, NULL, NULL, NULL) == -1) {
			perror("Erro na seleção de sockets");
			break;
		}

		if (FD_ISSET(serverSocket, &fds)) {
			acceptConnections(serverSocket);
		}

		if (FD_ISSET(serverSocket6, &fds)) {
			acceptConnections(serverSocket6);
		}
	}


}

void HTTPServer::acceptConnections(long long int server) {
	struct sockaddr_storage clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientSocket = accept(server, (struct sockaddr *) &clientAddr, &addrLen);
#ifdef SOCKET_NAO_BLOQUEANTE
	u_long mode = 1; // 1 para habilitar o modo não bloqueante
	if (ioctlsocket(clientSocket, FIONBIO, &mode) != NO_ERROR) {
		std::cerr << "Erro ao definir o socket do cliente para não bloqueante." << std::endl;
		closesocket(clientSocket);
		closesocket(serverSocket);
	}
#endif
	DWORD timeut = 3000;
	setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeut, sizeof(DWORD));
	if (clientSocket == -1) {
		perror("Erro ao aceitar conexão");
		return;
	}

	char clientIP[INET6_ADDRSTRLEN];
	if (clientAddr.ss_family == AF_INET) {
		struct sockaddr_in *clientAddrIPv4 = (struct sockaddr_in *) &clientAddr;
		inet_ntop(AF_INET, &clientAddrIPv4->sin_addr, clientIP, sizeof(clientIP));
	} else if (clientAddr.ss_family == AF_INET6) {
		struct sockaddr_in6 *clientAddrIPv6 = (struct sockaddr_in6 *) &clientAddr;
		inet_ntop(AF_INET6, &clientAddrIPv6->sin6_addr, clientIP, sizeof(clientIP));
	} else {
		printf("Tipo de endereço não suportado\n");
		return;
	}

	json::JSON request;
	request["client-ip"] = clientIP;

	processHttpRequest(clientSocket, request);
}

std::string HTTPServer::renderHtmlResponse(const char *html_file, int htmlError) {
	std::string html;
	FILE *file;

	fopen_s(&file, html_file, "r");
	char buff[1025];
	int len;

	while (!feof(file)) {
		len = fread(buff, 1, 1024, file);
		buff[len] = 0;
		html.append(buff);
	}
	if (html[0] == '\n') {
		html.insert(0, 1, '\r');
	}
	for (int i = 1; i < html.length(); ++i) {
		if (html[i] == '\n' && html[i - 1] != '\r') {
			html.insert(i, 1, '\r');
		}
	}

	return Response(htmlError, HTML, html);
}

std::string HTTPServer::Response(int code, ContentType type, const char *payload) {
	std::string spayload = payload;
	return Response(code, type, spayload);
}

std::string HTTPServer::Response(int code, json::JSON &payload) {
	return Response(code, JSON, payload.dump().c_str());
}

std::string HTTPServer::Response(int code, ContentType type, std::string &payload) {

	std::string response;
	switch (code) {
		case 200:
			response = "HTTP/1.1 200 OK\r\n";
			break;
		case 201:
			response = "HTTP/1.1 201 Created\r\n";
			break;
		case 204:
			response = "HTTP/1.1 204 No Content\r\n";
			break;
		case 400:
			response = "HTTP/1.1 400 Bad Request\r\n";
			break;
		case 401:
			response = "HTTP/1.1 401 Unauthorized\r\n";
			break;
		case 403:
			response = "HTTP/1.1 403 Forbidden\r\n";
			break;
		case 404:
			response = "HTTP/1.1 404 Not Found\r\n";
			break;
		case 500:
			response = "HTTP/1.1 500 Internal Server Error\r\n";
			break;
		default:
			response = "HTTP/1.1 500 Internal Server Error\r\n";
			return response;
	}
	response += "Content-Length: " + std::to_string(payload.length()) + "\r\n";
	response += "Content-Type: ";
	switch (type) {
		case HTML:
			response += "text/html; charset=utf-8\r\n";
		case JSON:
			response += "application/json\r\n";
		case XML:
			response += "application/xml\r\n";
		case Text:
			response += "text/plain\r\n";
		case Image:
			response += "image/jpeg\r\n"; // Exemplo de tipo de imagem
		default:
			response += "application/octet-stream\r\n"; // Tipo desconhecido
	}
	response += corsHeaders;
	response += "\r\n" + payload;
	return response;
}

#define PRINTLN(x)std::cout<<x<<std::endl;fflush(stdout);
#define PPRINTLN(x)std::cout<<"\t"<<x<<std::endl;fflush(stdout);

void HTTPServer::processHttpRequest(int client, json::JSON &request) {
	PRINTLN("Nova conexão")
	char buffer[5001];
	int bytesRead;
	std::string line;
	std::string response;
	std::string req;
	bytesRead = recv(client, buffer, 5001, 0);
	PPRINTLN("recebi " << bytesRead << " bytes")
	if (bytesRead <= 0) {
		PPRINTLN("Encerrei pq 0 bytes")
		CLOSE_SOCKET(client);
		return;
	}
	buffer[bytesRead] = 0;
	try {
		req = {buffer, (size_t) bytesRead};
		size_t pos;
		// descobrir methodo
		std::istringstream input_stream(req);


		std::getline(input_stream, line);
		PPRINTLN("REQ:" << line)
		pos = line.find(" HTTP/1.1");
		if (pos >= line.length()) {
			PPRINTLN("Encerrei pq não é HTTP/1.1");
			CLOSE_SOCKET(client);
			return;
		}
		std::string method = line.substr(0, pos);
		std::string params;
		pos = method.find('?');

		json::JSON headerParams = {};
		if (pos < method.length()) {
			params = method.substr(pos + 1);
			method = method.substr(0, pos);
			std::string key, value, param;
			while (params.length() > 0) {
				pos = params.find('&');
				param = params.substr(0, pos);
				if (pos == std::string::npos) {
					params = "";
				} else {
					params = params.substr(pos + 1);
				}
				pos = param.find('=');
				if (pos != std::string::npos) {
					key = param.substr(0, pos);
					value = param.substr(pos + 1);
					value+= "\n";
					PRINTLN("\tvalue:'"<<value<<"'");
					headerParams[key] = json::JSON::loadString(value);
				}
			}
		}
		request["params"] = headerParams;

		std::cout << "Request'" << method << "'" << std::endl;
		pos = method.find(' ');
		request["route"] = method.substr(pos + 1, method.length());

		if (!rotas.contains(method)) {
			//verificar se tem um path;
			std::cout << "Not found'" << method << "'" << std::endl;
			response = html_not_fount;
			send(client, response.c_str(), (int) response.length(), 0);
			CLOSE_SOCKET(client);
			return;
		}

		printf("Conexão aceita do cliente IP: %s\n", request["client-ip"].ToString().c_str());

		// ler request
		json::JSON header;
		json::JSON body;

		while (std::getline(input_stream, line)) {
			pos = line.rfind('\r');
			if (pos != std::string::npos)
				line.erase(pos, 1); // Remove 1 caractere a partir da posição pos	pos = line.find('\r');
			pos = line.rfind('\n');
			if (pos != std::string::npos)
				line.erase(pos, 1); // Remove 1 caractere a partir da posição pos
			if (line.empty())break;
			size_t colon_pos = line.find(':');
			if (colon_pos != std::string::npos) {
				std::string key = line.substr(0, colon_pos);
				std::string value = line.substr(colon_pos + 2); // +2 para ignorar o espaço após ':'
				header[key] = value;
			}
		}
		if (header["Content-Type"].ToString() == "application/json") {
			std::string json_raw;
			std::getline(input_stream, json_raw, '\0');
			PRINTLN(json_raw)
			body = json::JSON::loadString(json_raw);
		}
		request["header"] = header;
		request["body"] = body;

		response = rotas[method](*this, request);
		send(client, response.c_str(), (int) response.length(), 0);
	} catch (std::exception &e) {
		response = Response(500, Text, e.what());
		send(client, response.c_str(), (int) response.length(), 0);
	}
	CLOSE_SOCKET(client);
}

void HTTPServer::Rota(const char *method, const char *string, Method pFunction) {
	std::string rota = method;
	rota += " ";
	rota += string;
	this->rotas[rota] = pFunction;
}


