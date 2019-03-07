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
#ifndef QUESTION_FACTORY_HEADER
#define QUESTION_FACTORY_HEADER



class QuizFactory {

public:
	QuizFactory();

	//Question methods
	std::string QuestionOutput(QuestionId id);
	int		    QuestionMark(QuestionId id, std::string answer);

	//Quiz methods
	std::string QuizOutput(QuizId id);
	std::string	ListOfQuizs(void);

	//size_t		NumberQuestions(void) { return questions.size(); }

	bool Init(const char* dir);

	//void ModifyQuestion(StrongQuestionPtr pActor, tinyxml2::XMLElement* overrides);

private:

	//accessor methods
	StrongQuizPtr GetQuiz(QuizId id);

	std::map<QuizId, StrongQuizPtr> quizLookUpTable;

	StrongQuizPtr CreateQuiz(tinyxml2::XMLElement* quizData, std::string file);
	
};

#endif