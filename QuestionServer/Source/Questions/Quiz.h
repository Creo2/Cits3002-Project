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

class Quiz {
	friend class QuizFactory;

	QuestionId lastQuestionId;

public:
	Quiz(QuizId id);

	std::string AllQuestionOutput();
	std::string QuestionOutput(QuestionId id);
	int			QuestionMark(QuestionId id, std::string answer);
	size_t		NumberQuestions(void) { return questions.size(); }

	bool Init(tinyxml2::XMLElement* questions);
	
	//void ModifyQuestion(StrongQuestionPtr pActor, tinyxml2::XMLElement* overrides);
	
private:
	QuizId quizID;
	int numbMultipleChoice;
	int numbCoding;
		
	std::vector<StrongQuestionPtr> questions;
	
	QuestionId GetNextQuestionId(void) { ++lastQuestionId; return lastQuestionId; }

	virtual StrongQuestionComponentPtr VCreateComponent(tinyxml2::XMLElement* pData);

	StrongQuestionPtr CreateQuestion(tinyxml2::XMLElement* overrides,
		const QuestionId serversQuestionId);
};