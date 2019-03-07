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


class MultichoiceComponent : public QuestionComponent {
public:
	const static char *name;
	virtual const char *VGetName() const override { return MultichoiceComponent::name; }

public:
	MultichoiceComponent(void);

	// QuestionComponent interface
	virtual bool		VInit(tinyxml2::XMLElement* pData) override;
	virtual std::string VOutput(void) override;
	virtual int			VMark(std::string) override;

private:
	std::string answer;
	bool		random;
	size_t		display;
	std::vector<std::string> options;
};