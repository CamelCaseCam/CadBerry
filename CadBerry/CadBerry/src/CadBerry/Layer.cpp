#include "cdbpch.h"
#include "Layer.h"

namespace CDB
{
	Layer::Layer(const std::string& name) : m_Name(name)
	{

	}

	Layer::~Layer()
	{

	}

	void Layer::HeadlessInput()
	{
		std::cout << "Sorry, the layer \"" << m_Name << "\" does not support headless mode" << std::endl;
	}
}