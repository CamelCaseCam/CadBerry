#include <CadBerry.h>
#include <iostream>

int main(int argc, char** argv)
{
	bool headless = false;

	for (int i = 0; i < argc; ++i)
	{
		if (std::string(argv[i]) == "--headless")    //I know this is bad, but I don't care
		{
			headless = true;
			break;
		}
	}


	CDB::Application* MainApp = new CDB::Application(headless);

	MainApp->GetProject();
	MainApp->StartEditor();

	delete MainApp;
	return 0;
}