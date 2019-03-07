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

#include "Quiz.h"
#include "Question.h"
#include "QuestionComponent.h"
#include "MultichoiceComponent.h"
#include "CodingComponent.h"

Quiz::Quiz(QuizId id) {
	quizID = id;
	lastQuestionId = id;
	questions = { };
	numbCoding = 0;
	numbMultipleChoice = 0;
}

bool Quiz::Init(tinyxml2::XMLElement* pData) {
	printf("\tCreating Quiz: %d\n", quizID);

	if (!pData) {
		fprintf(stderr, "Warning - Cannot initialise Quiz %d: provided XML is empty\n",
			quizID);
		return false;
	}
	
	tinyxml2::XMLElement* questionXML = pData->FirstChildElement("Question");

	while (questionXML != NULL) {
		StrongQuestionPtr question = CreateQuestion(questionXML, GetNextQuestionId());
		if (question) {
			questions.push_back(question);
		}

		questionXML = questionXML->NextSiblingElement("Question");
	}

	if (questions.size() == 0) {
		fprintf(stderr, "Warning - Quiz %d has no questions\n",	quizID);
	}

	return true;
}

StrongQuestionPtr Quiz::CreateQuestion(tinyxml2::XMLElement* pData, 
									const QuestionId serversQuestionId) {
	if (!pData)	{
		fprintf(stderr, "Warning - Cannot create Question %d: provided XML is empty\n",
									serversQuestionId);
		return StrongQuestionPtr();
	}

	StrongQuestionPtr pQuestion = std::make_shared<Question>(serversQuestionId);
	if (!pQuestion->Init(pData)) {
		fprintf(stderr, "Warning - Cannot create Question %d: initialisation warning\n",
									serversQuestionId);
		return StrongQuestionPtr();
	}

	pQuestion->component = VCreateComponent(pData->FirstChildElement()->
									NextSiblingElement());
	if (!pQuestion->component) {
		fprintf(stderr, "Warning - Question %d could not be created: Initalisation Error\n",
			serversQuestionId);
		return StrongQuestionPtr();
	}

	pQuestion->component->SetOwner(pQuestion);
	pQuestion->type = pQuestion->component->VGetName();

	if (strcmp(pQuestion->type.c_str(), "Coding") == 0) {
		numbCoding++;
	} else {
		numbMultipleChoice++;
	}

	return pQuestion;
}

StrongQuestionComponentPtr Quiz::VCreateComponent(tinyxml2::XMLElement* pData) {
	const char* name = pData->Value();
	StrongQuestionComponentPtr pQuestionComp;

	if (strcmp(name, "MultipleChoice") == 0) {
		pQuestionComp = std::make_shared<MultichoiceComponent>();
	} else {
		if (strcmp(name, "Coding") == 0) {
			pQuestionComp = std::make_shared<CodingComponent>();
		} else {
			fprintf(stderr, "Warning - Component could not be created: Wrong type name\n");
			return StrongQuestionComponentPtr();
		}
	}

	if (!pQuestionComp->VInit(pData)) {
		fprintf(stderr, "Warning - Component could not be created: Initalisation Error\n");
		return StrongQuestionComponentPtr();
	}

	return pQuestionComp;
}

std::string Quiz::AllQuestionOutput() {
	
	std::string answer("");
	
	for (StrongQuestionPtr question : questions) {
		answer.append(question->Output());
		answer.append("&");
	}

	answer.pop_back();

	return answer;
}

std::string Quiz::QuestionOutput(QuestionId id) {
	if (id > lastQuestionId) {
		return std::string();
	}
	return questions[id - quizID - 1]->Output();
}

int Quiz::QuestionMark(QuestionId id, std::string answer) {
	if (id > lastQuestionId) {
		return WRONG_ID;
	}
	return questions[id - quizID - 1]->Mark(answer);
}