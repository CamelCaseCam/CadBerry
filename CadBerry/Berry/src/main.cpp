#include <CadBerry.h>

int main(int argc, char** argv)
{
	bool headless = false;

	for (int i = 0; i < argc; ++i)
	{
		if (argv[i] == "--headless")
		{
			headless = true;
		}
		else if (argv[i] == "--test")
		{
			//TODO: tests
		}
	}


	CDB::Application* MainApp = new CDB::Application(headless);

	MainApp->GetProject();
	MainApp->StartEditor();

	delete MainApp;
	return 0;
}