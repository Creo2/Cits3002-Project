//=============================================================================
//Project:		QuestionServer
//File:			
//Author:		Rhys Collier (22222918)
//CoAuthor:		Alexander Slate (21498906)
//Created:		12/05/2018
//Description: 	
//
//Modified on:	25/05/2018
//Modified by:	Rhys
//=============================================================================

#include "BaseHeader.h"
#include "QuestionServer.h"


void ListenLoop(Server* server, QuizFactory* quizs, bool* shouldShutDown) {
	while (!*shouldShutDown) {
		if (!server->ListenAndAccept(quizs)) {
			//sever has been shutdown due to an error.
			//With additional time would write code to reinitialise server.
			*shouldShutDown = true;
			return;
		} else {
			if (server->numberThreads() > 100) {
				server->JoinThread();
			}
		}
	}

	server->ServerShutdown();

	return;
}

QuestionServer::QuestionServer() {
	quizs = QuizFactory();
	server = Server();
	shouldShutdown = false;
}

bool QuestionServer::Setup(void) {
	printf("----------*Starting Question Server Setup*----------\n");

	//Get Current Directory and load settings file.
	char buf[DEFAULT_BUFLEN];
	GetCurrentDirectoryA(DEFAULT_BUFLEN, buf);

	tinyxml2::XMLDocument settings;

	if (settings.LoadFile("ProgramSettings.xml") != 0) {
		ErrorExit(TEXT("Program Setting file was not located in the same file as\
						 the executable\n"));
	}

	tinyxml2::XMLElement* rootElement = settings.RootElement();

	if (rootElement == NULL) {
		ErrorExit(TEXT("Nothing in ProgramSettings file, Terminating program\n"));
	}
	
	//Find for Quiz XML file directory and load Quizes
	tinyxml2::XMLElement* dirElement = rootElement->FirstChildElement("QuizDirectory");
	if (dirElement == NULL) {
		ErrorExit(TEXT("No quiz directory, Terminating program\n"));
	}
	std::string dir = dirElement->GetText();

	if (!quizs.Init(dir.c_str())) {
		ErrorExit(TEXT("No quizes were found in QuizDirectory. Please check values\
						in ProgramSetting.xml\n"));
	}

	//Get Server Settings and start server
	tinyxml2::XMLElement* serverElement = rootElement->FirstChildElement("Server");
	
	if (!server.Init(serverElement)) {
		ErrorExit(TEXT("Server could not be initialised with provided settings\n"));
	}

	listenThread = std::make_shared<std::thread>(ListenLoop, &server, &quizs, &shouldShutdown);

	printf("\n----------*Starting Question Server Setup Finished*----------\n");

	return true;
}


bool QuestionServer::CommandLoop() {
	std::string command;
	
	while (!shouldShutdown) {
		// get command
		printf("\nCommand: ");
		std::cin >> command;
		printf("\n");

		if (strncmp(command.c_str(), "shutdown", 8) == 0) {
			Shutdown();
		} else if (strncmp(command.c_str(), "list_quizs", 10) == 0) {
			ListOfQuizs();
		} else {
			printf("Not a valid command");
		}
	}

	listenThread->join();

	return true;
}