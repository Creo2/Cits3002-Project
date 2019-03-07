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

typedef std::string QuestionType;

class Question {
	friend class Quiz;

	private:
		QuestionId		questionID;
		QuestionType	type;
		StrongQuestionComponentPtr component;

		std::string		questionDesc; //Question Description

	public:
		explicit Question(QuestionId id);
		~Question(void);

		bool Init(tinyxml2::XMLElement* pData);

		// Accessors
		QuestionId   GetID(void) const { return questionID; }
		QuestionType GetType(void) const { return type; }
		

		// Operations
		bool		 Mark(std::string answer);
		std::string  Output(void);
};

