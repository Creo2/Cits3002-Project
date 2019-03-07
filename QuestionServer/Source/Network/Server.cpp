//=============================================================================
//Project:		QuestionServer
//File:			Server
//Author:		Rhys Collier (22222918)
//CoAuthor:		Alexander Slate (21498906)
//Created:		12/05/2018
//Description: 	File contains the code used for connection the question server
//				to the internet. It does this by first creating a TCP socket to
//				listen to and waits for connnections. Once a connection is 
//				established it secures it with openSSL and then creates a new 
//				thread to handle the client. In addition, the file contains a
//				function to process the input from the network.
//
//Modified on:	25/05/2018
//Modified by:	Rhys
//=============================================================================

#include "../BaseHeader.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

//Downloaded openssl 
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "../QuizFactory/QuizFactory.h"
#include "Server.h"

#pragma comment(lib, "Ws2_32.lib")

//Function:		ProcessServerInput
//Description:	Process the input and sends get answer.
//
//@param		input		Type char*
//@param		QuizFactory Type rhs
//
//@return		Type std::string answer
std::string ProcessServerInput(char* input, QuizFactory* quizs) {
	std::string answer("");

	if (strncmp(input, "Request=", 8) == 0) {
		input += 8; //move pointer forward to contense of request.
		
		//see if ID is the required length
		if (strlen(input) < ID_LENGTH) {
			answer.append("QuizID=");
			answer.append(input);
			
			answer.append("&invalidID");
			return answer;
		}

		if (strncmp(input, "0000000", ID_LENGTH) == 0) {
			//Request all quizs and their ids
			answer.append(quizs->ListOfQuizs());

			//check to see if request for all quiz questions or just a single question
		} else if (strncmp((input + 4), "000", 3) == 0) {
			//Request for quiz.
			std::string quizID(input, ID_LENGTH);
			
			int id = 0;

			try {
				id = std::stoi(quizID.c_str());
			}
			catch (std::exception const &e) {
				answer.append("Malformed\n");
				return answer;
			}

			std::string temp = quizs->QuizOutput(id);

			if (temp.size() == 0) {
				answer.append("QuizID=");
				answer.append(quizID);
				answer.append("&invalidID");
			}
			else {
				answer.append(temp);
			}
		}
		else {
			//Request for question.
			std::string questionID(input, ID_LENGTH);

			int id = 0;
			try {
				id = std::stoi(questionID.c_str());
			}
			catch (std::exception const &e) {
				answer.append("Malformed\n");
				return answer;
			}

			std::string temp = quizs->QuestionOutput(id);

			//check to see if something was return.
			if (temp.size() == 0) {
				answer.append("QuestionID=");
				answer.append(questionID);
				answer.append("&InvalidID");
			}
			else {
				answer.append(temp);
			}
		}

	//See if a mark request
	} else if (strncmp(input, "Mark=", 5) == 0) {
		input += 5; //move pointer forward to contense of mark request.

		std::string questionID(input, ID_LENGTH);

		//move forward and check to see if answer is provided.
		input += ID_LENGTH;
		if (std::strncmp(input, "&Answer=", 8) == 0) {
			input += 8;

			std::string inputAnswer(input);

			int id = 0;
			try {
				id = std::stoi(questionID.c_str());
			}
			catch (std::exception const &e) {
				answer.append("Malformed\n");
				return answer;
			}

			int result = quizs->QuestionMark(id, inputAnswer);

			answer.append("QuestionID=");
			answer.append(questionID);
			answer.append("&Result=");

			//add result and check for errors
			if (result == TRUE) {
				answer.append("True");
			}
			else if (result == FALSE) {
				answer.append("False");
			}
			else if (result == WRONG_ID) {
				answer.erase(8);//remove "&Result="
				answer.append("&InvalidID");
			} else if (result == SERVER_ERROR) {
				answer.erase(8);//remove "&Result="
				answer.append("&Error");
			} else {
				//not option or answer
				answer.erase(8);//remove "&Result="
				answer.append("&InvalidAnswer");
			}
		}
		else {
			answer.append("Malformed");
		}

		//Neither mark request or request, Malformed message. 
	}
	else {
		answer.append("Malformed");
	}
	answer += '\n';
	return answer;
}

//Function:		clientLoop
//Description:	Secure SSL socket is used to
//
//@param		input		Type char*
//@param		QuizFactory Type rhs
//
//@return		Type std::string answer
void clientLoop(UINT_PTR clientSocket, void* sslTemp, QuizFactory* quizs) {
	int iResult, iSendResult; 
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	SSL* ssl = (SSL *)sslTemp;

	printf("\rNew thread to handle client.......\n");

	do {
		printf("\rLooping.......\n");
		//No SSL recieve
		//iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		
		//SSL recieve
		iResult = SSL_read(ssl, recvbuf, recvbuflen);
		
		if (iResult > 0) {
			printf("\rBytes received: %d\n", iResult);

			//process input
			recvbuf[iResult] = '\0';
			std::string returnMessage = ProcessServerInput(recvbuf, quizs);
			printf("\n\nsending the following: %s\n\n", returnMessage.c_str());
			printf("Bytes to send: %d\n", returnMessage.size());
			
			//No SSL send answer back to client.
			//iSendResult = send(clientSocket, returnMessage.c_str(), returnMessage.size(), 0);
			//if (iSendResult == SOCKET_ERROR) {
			//	DisplayErrorBox(TEXT("Send failed\n"));
			//	closesocket(clientSocket);
			//	return;
			//}

			//SSL send answer back to client.
			iSendResult = SSL_write(ssl, returnMessage.c_str(), returnMessage.size());
			if (iSendResult < 0) {
				printf("SSL_write failed with error: %d\n", SSL_get_error(ssl, iResult));
				SSL_free(ssl);
				closesocket(clientSocket);
				return;
			}
			
			printf("Bytes sent: %d\n", iSendResult);

		} else if (iResult == 0) {
			printf("Connection closing...\ncommand: ");
		} else {
			//Error has occured
			// No SSL error reporting
			//printf("recv failed with error: %d\n", WSAGetLastError());
			
			//SSL error reporting
			printf("SSL_read failed with error: %d\n", SSL_get_error(ssl, iResult));
			SSL_free(ssl);
			closesocket(clientSocket);
			return;
		}

	} while (iResult > 0);

	iResult = shutdown(clientSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		DisplayErrorBox(TEXT("Shutdown of client failed\n"));
	}
	
	SSL_free(ssl);
	closesocket(clientSocket);
	return;
}

Server::Server() {
	listenSocket = INVALID_SOCKET;
	backLog = SOMAXCONN;
	clientSocketThreads = {};
	listenPort = std::string();
	ctx = NULL;
}

bool Server::Init(tinyxml2::XMLElement* serverElement) {
	//process tempBackLog
	printf("\n----------*Initializing the server!*----------\n");
	printf("\t Retrieve Server settings....\n");
	
	//get port
	const char* port = serverElement->Attribute("listenPort");

	if (port == NULL) {
		DisplayErrorBox(TEXT("Server Settings: No port value provided\nCheck ProgramSettings.xml"));
	}

	listenPort.append(port);

	printf("\t\tServer port = %s\n", port);

	//get Backlog value. 
	const char* tempBackLog = serverElement->Attribute("backLog");
	if (tempBackLog == NULL) {
		DisplayErrorBox(TEXT("Server Settings: No backlog value provided\nCheck ProgramSettings.xml"));
	}

	if (strcmp(tempBackLog, "SOMAXCONN") == 0) {
		backLog = SOMAXCONN;
	}
	else {
		try {
			backLog = std::stoi(tempBackLog);
		}
		catch (std::exception const &e) {
			DisplayErrorBox(TEXT("Invalid backlog value\n"));
			return false;
		}
	}

	printf("\t\tbacklog value = %d\n", backLog);

	//get SSL certificate file location
	const char* certificate = serverElement->Attribute("certificateFile");
	if (certificate == NULL) {
		DisplayErrorBox(TEXT("Server Settings: No key file location\nCheck ProgramSettings.xml"));
	}
	printf("\t\tServer certificate location = %s\n", certificate);


	//get SSL key file location
	const char* key = serverElement->Attribute("privateKeyFile");
	if (key == NULL) {
		DisplayErrorBox(TEXT("Server Settings:  No key file location\nCheck ProgramSettings.xml"));
	}
	printf("\t\tServer key location = %s\n", key);

	//Initiates openSSL
	if (!initSSL(key, certificate)) { return false; }

	if (!initWinSock()) { return false; }
	
	//When the server is actually initated.
	if (!initListenSocket(port, tempBackLog)) { return false; }
		
	return true;
}

bool Server::initSSL(const char* key, const char* certificate) {
	//Start up open SSL 
	printf("\t Initialising openSSL\n");
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();

	const SSL_METHOD* method = SSLv23_server_method();

	SSL_CTX* tempctx = SSL_CTX_new(method);

	if (!tempctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		DisplayErrorBox(TEXT("Unable to create SSL context\n"));
		return false;
	}


	SSL_CTX_set_ecdh_auto(ctx, 1);

	// Set the key and cert
	if (SSL_CTX_use_certificate_file(tempctx, certificate, SSL_FILETYPE_PEM) <= 0) {
		printf("This failed :(\n");
		ERR_print_errors_fp(stderr);
		DisplayErrorBox(TEXT("Unable to load SSL certificate\n"));
		return false;
	}

	if (SSL_CTX_use_PrivateKey_file(tempctx, key, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		DisplayErrorBox(TEXT("Unable to load SSL key\n"));
		return false;
	}

	ctx = (void *)tempctx;

	return true;
}

bool Server::initWinSock() {
	printf("\t Initialising Winsock\n");
	//WinSock Initialisation.
	WSADATA wsaData; //required for winsock

	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		DisplayErrorBox(TEXT("WSAStartup failed\n"));
		return false;
	}

	return true;
}

bool Server::initListenSocket(const char* port, const char* tempBackLog) {
	struct addrinfo hints, *serverInfo = NULL, *ptr = NULL;
	int iResult;
	//char yes = 1;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_protocol = IPPROTO_TCP; //Use TCP protocols 
	//hints.ai_flags = AI_PASSIVE; // fill in my IP for me

								 // Resolve the local address and port to be used by the server
	iResult = getaddrinfo("192.168.43.123", port, &hints, &serverInfo);
	if (iResult != 0) {
		DisplayErrorBox(TEXT("getaddrinfo failed\n"));
		WSACleanup();
		return false;
	}// serverInfo now points to a linked list of 1 or more struct addrinfos


	 //create the socket to listen for connections
	listenSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		char message[BUFSIZ];
		sprintf_s(message, "Error while creating socket\n");
		DisplayErrorBox(TEXT(message));

		freeaddrinfo(serverInfo);
		WSACleanup();
		return false;
	}

	/*iResult = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(char));
	if (iResult == -1) {
	DisplayErrorBox(TEXT("setsockopt error\n"));
	freeaddrinfo(serverInfo);
	WSACleanup();
	return false;
	}*/

	// Setup the TCP listening socket by associating port.
	iResult = bind(listenSocket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		DisplayErrorBox(TEXT("bind failed\n"));

		freeaddrinfo(serverInfo);
		closesocket(listenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(serverInfo); // free the linked-list as serverInfo is no longer needed. 

	char name[256];

	gethostname(name, 256);

	printf("\t\tServer host name: %s", name);

	return true;
}

bool Server::ListenAndAccept(QuizFactory* quizs) {
	int iResult;

	printf("\rStarting to listen\n command: ");

	//Listen for client
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		DWORD error = GetLastError();

		if (error == WSAENETDOWN || WSAEADDRINUSE) {
			DisplayErrorBox(TEXT("listen failed\n"));
			ServerShutdown();
			return false;
		}

		DisplayErrorBox(TEXT("listen failed\n"));

		return false;
	}

	printf("\rFound Someone.....\n");

	// Accept a client socket
	SOCKET clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) {
		DWORD error = GetLastError();
		printf("Error is: %s\n", error);
		if (error == WSAENETDOWN) {
			DisplayErrorBox(TEXT("Accept failed\n"));
			ServerShutdown();
			return false;
		}
		DisplayErrorBox(TEXT("Accept failed\n"));
		return false;
	}

	printf("\rTrying to connect checking SSL.......\n");

	//Connect the SSL socket
	SSL_CTX* tempctx = (SSL_CTX *)ctx;
	ssl_st* ssl = SSL_new(tempctx);
	SSL_set_fd(ssl, clientSocket);
	iResult = SSL_accept(ssl);
	if (iResult <= 0) {
		//did not accept display reason. For now have use dialog box.
		fprintf(stderr, "\rSSL Error was: %d\n", SSL_get_error(ssl, iResult));
		DisplayErrorBox(TEXT("SSL accept failed:"));
		return true; //SSL failed but server did not die.
	} else {
		//create Thread for new client and process
		printf("ssl accepted\n");
		StrongthreadPtr clientThread;

		clientThread = std::make_shared<std::thread>(clientLoop , clientSocket, ssl, quizs);
		clientSocketThreads.push_back(clientThread);
	}

	printf("New thread created to handle client\n Command: ");
	
	return true;
}

int Server::JoinThread() {
	for (int i = clientSocketThreads.size() -1; i >= 0; i--) {
		clientSocketThreads[i]->join();
		clientSocketThreads.pop_back();
	}

	return 0;
}


void Server::ServerShutdown(void) {
	closesocket(listenSocket);

	JoinThread();

	WSACleanup();

	return;
}
