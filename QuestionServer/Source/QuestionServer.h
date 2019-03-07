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

#pragma once
#include "QuizFactory/QuizFactory.h"
#include "Network/Server.h"

void	ListenLoop(Server* server, QuizFactory* quizs, bool* shouldShutDown);

class QuestionServer {
public:
	QuestionServer(void);

	// QuestionComponent interface
	bool	Setup();
	bool	CommandLoop();


private:
	bool shouldShutdown = false;
	QuizFactory		quizs;
	Server			server;
	StrongthreadPtr listenThread;
	

	void	Shutdown()	  { shouldShutdown = true; }
	void	ListOfQuizs()    { printf("%s", quizs.ListOfQuizs().c_str()); }
};