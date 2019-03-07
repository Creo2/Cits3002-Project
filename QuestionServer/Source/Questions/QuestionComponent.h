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

#ifndef QUESTION_COMPONENT_HEADER
#define QUESTION_COMPONENT_HEADER


class QuestionComponent {
	friend class Quiz;

protected:
	StrongQuestionPtr pOwner;

public:
	virtual ~QuestionComponent(void) { pOwner.reset(); }

	// These functions are meant to be overridden by the implementation classes of the components.
	virtual bool		VInit(tinyxml2::XMLElement* pData) { fprintf(stderr, "Error: blank\n"); return false; };
	virtual void		VOnChanged(void) { }				// [mrmike] - added post-press
	virtual std::string VOutput(void) { return NULL; }
	virtual int			VMark(std::string) = 0;

	virtual const char *VGetName() const = 0;

private:
	void SetOwner(StrongQuestionPtr pOwner) { pOwner = pOwner; }

};

#endif