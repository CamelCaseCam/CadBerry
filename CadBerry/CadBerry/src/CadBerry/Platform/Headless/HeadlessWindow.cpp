#include <cdbpch.h>
#include "HeadlessWindow.h"
#include <iostream>

namespace CDB
{
	using std::cout, std::cin, std::endl;


	const char* HelpString = 
R"(Type "project" to enter project submenu
Type "windows" to enter window submodule
Type "window list" to get the list of open windows
Type the name of any open window to view that window's options
Type "help" to view this prompt again)";
	HeadlessWindow::HeadlessWindow(const WindowProps& props)
	{
		cout << 
R"(###############################################################
                   CadBerry headless mode
###############################################################

)" << HelpString << endl;
	}

	HeadlessWindow::~HeadlessWindow()
	{
		cout << "\nExiting CadBerry headless mode" << endl;
	}

	//Forward declarations
	void ProjectMenu();
	void WindowMenu();

	void HeadlessWindow::OnUpdate()
	{
		for (Layer* layer : this->m_LayerStack)
		{
			layer->HeadlessInput();
		}
	}

	void HeadlessWindow::Init(const WindowProps& props)
	{
	}

	void HeadlessWindow::Shutdown(bool PreserveGLFW)
	{
	}
}