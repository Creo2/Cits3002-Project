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

#include "Question.h"
#if __has_include("QuestionComponent.h")
#include "QuestionComponent.h"
#endif


Question::Question(QuestionId id) {
	questionID = id;
	type = "Unknown";

}

bool Question::Init(tinyxml2::XMLElement* pData) {
	printf("\t\tInitializing Question %d\n", questionID);

	tinyxml2::XMLElement* description = pData->FirstChildElement("Description");

	if (description == NULL) {
		fprintf(stderr, "Warning - Question %d has no description in XML\n", questionID);
		return false;
	}

	questionDesc = description->GetText();
	RemoveTabs(&questionDesc);

	return true;
}

std::string Question::Output(void) {
	std::string output = "questionID=";
	output.append(std::to_string(questionID) += '&');

	output.append(std::string("type=").append(type) += '&');

	output.append(std::string("description=").append(questionDesc));
	output += '&';

	if (type == "Multichoice") {
		output += '&';
	}

	output.append(component->VOutput());

	return output;
}

bool Question::Mark(std::string answer) {
	return component->VMark(answer);
}

Question::~Question(void) {
	if (component) { component->~QuestionComponent(); }
}
