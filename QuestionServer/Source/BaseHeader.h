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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//Standard C/C++ headers
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <iostream>
#include <stdexcept>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <thread>

#include <strsafe.h>
#include <windows.h>
#include <Shlwapi.h>

//downloaded Libraries
#include "Includes/tinyxml2.h"
//#include <Python.h>

#define DEFAULT_BUFLEN  512
#define EXTENDED_BUFLEN 2048

#define WRONG_ID -1
#define SERVER_ERROR -2
#define NOT_OPTION -3

#define ID_LENGTH 7

class Question;
typedef std::shared_ptr<Question> StrongQuestionPtr;
typedef unsigned long QuestionId;

class QuestionComponent;
typedef std::shared_ptr<QuestionComponent> StrongQuestionComponentPtr;
typedef unsigned long ComponentId;

class Quiz;
typedef std::shared_ptr<Quiz> StrongQuizPtr;
typedef unsigned long QuizId;

void RemoveTabs(std::string* text);
void AddForwardSlash(std::string* text);




//Windows Specific Functions. 
std::vector<std::string> getXMLsInDirectory(std::string dir);
void ErrorExit(PTSTR lpszFunction);
void DisplayErrorBox(LPTSTR lpszFunction);
bool CreateAndWriteFile(const char* fileName, std::string* fileInput);
bool ReadFile(const char* fileName, std::string* Output);