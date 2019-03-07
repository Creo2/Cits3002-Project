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

#if __has_include("../BaseHeader.h")
#include "../BaseHeader.h"
#endif

#include "QuizFactory.h"
#include "../Questions/Quiz.h"

QuizFactory::QuizFactory() {
	quizLookUpTable = {};
}

bool QuizFactory::Init(const char* dir) {
	printf("\n----------*Initializing Quizes!*----------\n");

	std::vector<std::string> files = getXMLsInDirectory(dir);

	for (std::string file : files) {
		tinyxml2::XMLDocument quizs;

		if (quizs.LoadFile(file.c_str()) != 0) {
			char message[BUFSIZ];
			sprintf_s(message, "Error: could not open %s.\n Please check input data and ensure proper XML format\n", file.c_str());
			DisplayErrorBox(TEXT(message));
			continue;
		}

		tinyxml2::XMLElement* rootElement = quizs.RootElement();

		if (rootElement == NULL) {
			char message[BUFSIZ];
			sprintf_s(message, "Warning: Nothing in file %s\n", file.c_str());
			DisplayErrorBox(TEXT(message));
			continue;
		}

		while (rootElement != NULL) {
			StrongQuizPtr newQuiz = CreateQuiz(rootElement, file.c_str());
			if (newQuiz != NULL) {
				quizLookUpTable.insert(std::pair<QuizId, StrongQuizPtr>(newQuiz->quizID, newQuiz));
			}
			rootElement = rootElement->NextSiblingElement("Quiz");
		}
	}

	if (quizLookUpTable.size() == 0) {
		return false;
	}

	printf("\n----------*Initializing Quizes Done!*----------\n");

	return true;
}

StrongQuizPtr QuizFactory::CreateQuiz(tinyxml2::XMLElement* quizData, std::string file) {
	int quizID;

	tinyxml2::XMLElement* IDElement = quizData->FirstChildElement("QuizId");

	if (IDElement == NULL) {
		char message[BUFSIZ];
		sprintf_s(message, "Warning: Quiz in %s has no ID\n", file.c_str());
		DisplayErrorBox(TEXT(message));
		return StrongQuizPtr();
	}

	IDElement->QueryIntAttribute("v", &quizID);

	StrongQuizPtr newQuiz = std::make_shared<Quiz>(quizID);

	if (!newQuiz->Init(quizData)) {
		char message[BUFSIZ];
		sprintf_s(message, "Warning: Quiz %d in %s could not be initialised\n", quizID, file.c_str());
		DisplayErrorBox(TEXT(message));
		return StrongQuizPtr();
	}
	
	return newQuiz;
}

StrongQuizPtr QuizFactory::GetQuiz(QuizId id) {
	//See if quiz id is in table of quizes if not then return an empty string
	StrongQuizPtr quiz;

	try {
		quiz = quizLookUpTable.at(id);
	}
	catch (const std::out_of_range& oor) {
		return NULL;
	}

	return quiz;
}

std::string QuizFactory::QuestionOutput(QuestionId id) {
	QuizId quizID = QuizId((id / 1000) * 1000);
	StrongQuizPtr quiz = GetQuiz(quizID);

	if (quiz == NULL) {
		return std::string();
	}

	return quiz->QuestionOutput(id);
}

int QuizFactory::QuestionMark(QuestionId id, std::string answer) {
	QuizId quizID = QuizId((id / 1000) * 1000);
	StrongQuizPtr quiz = GetQuiz(quizID);

	if (quiz == NULL) {
		return WRONG_ID;
	}

	return quiz->QuestionMark(id, answer);
}

std::string QuizFactory::QuizOutput(QuizId id) {
	
	StrongQuizPtr quiz = GetQuiz(id);

	if (quiz == NULL) {
		return std::string();
	}

	return quiz->AllQuestionOutput();
}

std::string QuizFactory::ListOfQuizs(void) {

	std::string answer = "";

	for (std::pair<QuizId, StrongQuizPtr> quiz : quizLookUpTable) {	
		answer.append("QuizId=");
		answer.append(std::to_string(quiz.first));
		answer.append("&MultipleChoice=");
		answer.append(std::to_string(quiz.second->numbMultipleChoice));
		answer.append("&Coding=");
		answer.append(std::to_string(quiz.second->numbCoding));
		answer.append("&");
	}

	answer.pop_back();

	return answer;
}