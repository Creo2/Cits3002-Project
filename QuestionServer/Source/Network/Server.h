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

#pragma once

typedef std::shared_ptr<std::thread> StrongthreadPtr;

class QuizFactory;
std::string ProcessServerInput(char* input, QuizFactory* quizs);
void clientLoop(UINT_PTR clientSocket, void* sslTemp, QuizFactory* quizs);


class Server {
public:
	Server(void);

	// QuestionComponent interface
	bool	Init(tinyxml2::XMLElement* serverElement);
	void	ServerShutdown();
	bool	ListenAndAccept(QuizFactory* quizs);
	int		JoinThread();
	int		numberThreads() { return clientSocketThreads.size(); }
	
	
private:
	UINT_PTR		listenSocket; //socket to listen to connections on. 
	int				backLog;
	std::string		listenPort;
	void*			ctx;
	
	std::vector<StrongthreadPtr> clientSocketThreads;
	std::map<std::thread*, bool> threadDone;

	bool initSSL(const char* key, const char* certificate);
	bool initWinSock(void);
	bool initListenSocket(const char* port, const char* tempBackLog);

};