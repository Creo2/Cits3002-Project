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

#include "QuestionComponent.h"

class CodingComponent : public QuestionComponent {
public:
	const static char *name;
	virtual const char *VGetName() const override { return CodingComponent::name; }

public:
	CodingComponent(void);

	// QuestionComponent interface
	virtual bool		VInit(tinyxml2::XMLElement* pData) override;
	virtual std::string VOutput(void) override { std::string output = ""; return output; }
	virtual int			VMark(std::string) override;

private:
	std::vector<std::string> testsInputs;  //stores inputs
	std::vector<std::string> testsOutputs; //stores outputs 
	std::string		initCode;
};