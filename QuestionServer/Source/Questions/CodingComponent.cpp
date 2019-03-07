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
#include "CodingComponent.h"


const char *CodingComponent::name = "Coding";

CodingComponent::CodingComponent(void) {
	testsInputs = {};
	testsOutputs = {};
	initCode = "";
}

bool CodingComponent::VInit(tinyxml2::XMLElement* pData) {
	printf("\t\t\tCoding Question\n");
	if (!pData) {
		fprintf(stderr, "\t\tWarning - Cannot create Coding Question: no data\n");
		return false;
	}


	//Grab additional code to be added to the provided answer before compiling.
	
	tinyxml2::XMLElement* initCodeXML = pData->FirstChildElement("InitCode");

	if (initCodeXML == NULL) {
		fprintf(stderr, "\t\tWarning - Cannot create Coding Question: no initCode provided\n");
		return false;
	}

	initCode.append(initCodeXML->Attribute("v"));

	if (initCode.compare("") == 0) {
		fprintf(stderr, "\t\tWarning - Cannot create Coding Question: no initCode provided\n");
		return false;
	}

	if (initCode.compare("NULL") == 0) {
		initCode.clear();
	}
	
	//Grab test information and store it.
	tinyxml2::XMLElement* test = pData->FirstChildElement("Test");
	while (test != NULL) {
		
		//get input
		tinyxml2::XMLElement* temp = test->FirstChildElement("Input");
		if (temp == NULL) {
			fprintf(stderr, "\t\tWarning - Cannot create Coding Question: no input for a coding test\n");
			return false;
		}
		testsInputs.push_back(temp->GetText());
		RemoveTabs(&testsInputs[testsInputs.size() - 1]);
		AddForwardSlash(&testsInputs[testsInputs.size() - 1]);

		//get output
		temp = test->FirstChildElement("Output");
		if (temp == NULL) {
			fprintf(stderr, "\t\tWarning - Cannot create Coding Question: no output for a coding test\n");
			return false;
		}
		testsOutputs.push_back(temp->GetText());
		RemoveTabs(&testsOutputs[testsOutputs.size() - 1]);
		

		test = test->NextSiblingElement("Test");
		
	}

	if (testsInputs.size() == 0) {
		fprintf(stderr, "\t\tWarning - Cannot create Coding Question : no tests\n");
		return false;
	}

	return true;
}

int CodingComponent::VMark(std::string answer) {

	//check answer for banned commmands.
	//Todo: store this in a xml file.
	std::vector<std::string> illegalcommands = { "system",
												"#include",
												"CreateFile",
												"OpenFile"
											   }; 

	for (std::string command : illegalcommands) {
		if (answer.find(command) != std::string::npos) {
			return FALSE;
		}
	}

	std::string code = "";
	code.append(initCode);
	code.append(answer);

	std::string codeFileName = "codefile";
				codeFileName.append(std::to_string(rand()));
				codeFileName.append(".c");

	if (!CreateAndWriteFile(codeFileName.c_str(), &code)) {
		return SERVER_ERROR;
	}

	std::string oldFile = "";
	oldFile.append(codeFileName);

	//compile file. Run the solution and see if there is an output.
	std::string command = "g++ ";
				command.append(codeFileName);
				command.append(" -o ");
				codeFileName.pop_back();
				codeFileName.append("exe");
				command.append(codeFileName);

	printf("\rRunning the following command: %s\n", command);
	system(command.c_str());

	DeleteFile(oldFile.c_str());

	//check to see if file exists if not then compiling error has occured.
	if (!PathFileExists(codeFileName.c_str())) {
		return FALSE;
	}

	//create input and output files.
	std::string testFile = "testInput";
	testFile.append(std::to_string(rand()));
	testFile.append(".txt");

	std::string OutputFile = "testOutput";
				OutputFile.append(std::to_string(rand()));
				OutputFile.append(".txt");

	command.clear();
	command.append(codeFileName);
	command.append(" < ");
	command.append(testFile);
	command.append(" > ");
	command.append(OutputFile);

	std::string result = "";

	for (int i = 0; i < testsInputs.size(); i++) {
		//creates new file will overwrite previous one.
		if (!CreateAndWriteFile(testFile.c_str(), &testsInputs[i])) {
			return SERVER_ERROR;
		}
		printf("\rRunning the following command: %s\n", command);
		system(command.c_str());

		if (!PathFileExists(OutputFile.c_str())) {
			//Cleanup.
			DeleteFile(codeFileName.c_str());
			DeleteFile(OutputFile.c_str());
			DeleteFile(testFile.c_str());
			return SERVER_ERROR;
		}

		if (!ReadFile(OutputFile.c_str(), &result)) {
			//Cleanup.
			DeleteFile(codeFileName.c_str());
			DeleteFile(OutputFile.c_str());
			DeleteFile(testFile.c_str());
			return SERVER_ERROR;
		}

		printf("\rResult: %s\n", result.c_str());
		if (testsOutputs[i].compare(result) != 0) {
			//wrong answer report as such. Cleanup.
			DeleteFile(codeFileName.c_str());
			DeleteFile(OutputFile.c_str());
			DeleteFile(testFile.c_str());
			return FALSE;
		}
	}

	//got to this point all answers were correct. Cleanup

	DeleteFile(codeFileName.c_str());
	DeleteFile(OutputFile.c_str());
	DeleteFile(testFile.c_str());

	return TRUE;
	
	//PYTHON CHECKING CODE! THIS DID NOT WORK WHEN USING THREADS :(
	//this is python code to redirect stdouts/stderr
	/*
	std::string stdOutErrPython =
"import sys\n\
import io\n\
class CatchOutErr:\n\
    def __init__(self):\n\
        self.value = ''\n\
    def write(self, txt):\n\
        self.value += txt\n\
catchOutErr = CatchOutErr()\n\
sys.stdout = catchOutErr\n\
sys.stderr = catchOutErr\n"; 
	//this is python code to preallocate some stdin
	std::string stdInPython =
"s = io.StringIO(\'\')\n\
sys.stdin  = s\n";

	if (answer.compare("") == 0) {

		return FALSE;
	}

	
	for (int i = 0; i < testsInputs.size(); i++) {
		std::string inputPythonCode = "";
		inputPythonCode.append(stdInPython);
		inputPythonCode.insert(17, testsInputs[i]);

		Py_Initialize();

		printf("%s\n done\n", answer.c_str());

		PyObject *pModule = PyImport_AddModule("__main__"); //create main module
		PyRun_SimpleString(stdOutErrPython.c_str()); //invoke code to redirect
		PyRun_SimpleString(inputPythonCode.c_str()); //code to allocate some input
		answer.push_back('\n');
		
		PyRun_SimpleString(answer.c_str()); 
		PyObject *catcher = PyObject_GetAttrString(pModule, "catchOutErr"); //get our catchOutErr created above

		PyObject *output = PyObject_GetAttrString(catcher, "value"); //get the stdout and stderr from our catchOutErr object

		if (output == NULL) {
			printf("Error: output variable was not found!: Something Wrong with python stdout redirect\n");
			Py_Finalize();

			return FALSE;
		}

		std::string result = PyUnicode_AsUTF8(output);
		result.pop_back();
		printf("\nResult: %s\n", result.c_str());
		if (testsOutputs[i].compare(result) != 0) {
			Py_Finalize();

			return FALSE;
		}

		

		Py_Finalize();
	}
	
	return TRUE;
	*/
}

