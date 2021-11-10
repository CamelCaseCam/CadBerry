#include <CadBerry.h>

int main()
{
	CDB::Application* MainApp = new CDB::Application();

	MainApp->GetProject();
	MainApp->StartEditor();

	delete MainApp;
	return 0;
}