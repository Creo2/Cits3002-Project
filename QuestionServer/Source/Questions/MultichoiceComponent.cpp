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

#include "MultichoiceComponent.h"

const char *MultichoiceComponent::name = "MultichoiceComponent";

MultichoiceComponent::MultichoiceComponent(void) {
	answer = "";
	options = {};
	random = false;
	display = 0;
}

bool MultichoiceComponent::VInit(tinyxml2::XMLElement* pData) {
	printf("\t\t\tMultiChoice Question\n");
	if (!pData) {
		fprintf(stderr, "Warning - Cannot create Multichoice Question: no data\n");
		return false;
	}

	answer = pData->FirstChildElement("Answer")->GetText();

	if (answer.compare("") == 0) {
		fprintf(stderr, "Warning - Cannot create Multichoice Question : no answer\n");
		return false;
	}

	tinyxml2::XMLElement* temp = pData->FirstChildElement("Random");

	if (temp) {
		int temp2 = 0;
		if (temp->QueryIntAttribute("v", &temp2) == 0) {
			random = temp2 != 0;
			printf("\t\t\t\tRandom = true\n");
		} else {
			fprintf(stderr, "Warning - Cannot create Multichoice Question : missing value for Random\n");
			return false;
		}
	}

	temp = pData->FirstChildElement("Display");
	if (temp) {
		int temp2 = 0;
		if (temp->QueryIntAttribute("v", &temp2) == 0) {
			display = temp2;
			printf("\t\t\t\tdisplay = %i\n", display);
		} else {
			fprintf(stderr, "Warning - Cannot create Multichoice Question : missing value for Display\n");
			return false;
		}
	}

	if (!temp) {
		fprintf(stderr, "Warning - Cannot create Multichoice Question: missing display\n");
		return false;
	} 

	tinyxml2::XMLElement* option = pData->FirstChildElement("Choice");
	while (option != NULL) {
		options.push_back(option->GetText());
		option = option->NextSiblingElement("Choice");
	}

	if (options.size() == 0) {
		fprintf(stderr, "Warning - Cannot create Multichoice Question : no options\n");
		return false;
	}

	return true;
}

std::string MultichoiceComponent::VOutput(void) {
	std::string output = "";

	size_t answerPos = rand() % (options.size() + 1);

	short i = 0;

	if (answerPos == 0) {
		output.append("option=");
		output.append(answer);
		output += '&';
	}
	for (std::string option : options) {
		output.append("option=");
		output.append(option);
		output += '&';
		i++;
		if (answerPos == i) {
			output.append("option=");
			output.append(answer);
			output += '&';
		}
	}
	output.pop_back();
	return output;
}

int MultichoiceComponent::VMark(std::string proposedAnswer) {


	printf("\n%s\t%s\n", answer.c_str(), proposedAnswer.c_str());
	if (answer.compare(proposedAnswer) != 0) {
		for (std::string option : options) {
			if (option.compare(proposedAnswer) == 0) { return FALSE; }
		}
		//proposedAnswer is not an option or answer.
		//Something wrong has occured.
		return NOT_OPTION;
	}

	return TRUE;
}