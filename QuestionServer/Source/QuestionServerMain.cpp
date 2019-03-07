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
#include <strsafe.h>
#include <tchar.h>
#include "QuestionServer.h"


int main(int argc, char **argv) {
	
	QuestionServer server;

	server.Setup();

	server.CommandLoop();

	/*
	printf("%s\n\n\n", quizs.ListOfQuizs().c_str());

	//printf("%s\n\n", quizs.QuestionOutput(2913001).c_str());

	//printf("%s\n\n", quizs.QuestionOutput(2913002).c_str());

	//printf("%s\n\n", quizs.QuestionOutput(2913005).c_str());

	if (quizs.QuestionMark(2913001, "H.P. Lovecraft, The Call of Cthulhu") == 1) {
		printf("You're right!\n");
	} else {
		printf("You're wrong!\n");
	}

	if (quizs.QuestionMark(2913004, "inp1 = input()\nprint(int(inp1))\n") == 1) {
		printf("You're right!\n");
	}
	else {
		printf("You're wrong!\n");
	}

	if (quizs.QuestionMark(2913005, "inp1 = input()\ninp2 = input()\nprint(int(inp1)+int(inp2))\n") == 1) {
		printf("You're right!\n");
	} else{
		printf("You're wrong!\n");
	}
	*/
	exit(EXIT_SUCCESS);
}