#pragma once
#include "CadBerry/Core.h"
#include "CadBerry/Layer.h"

#include <vector>

namespace CDB
{
	class CDBAPI LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }
	private:
		std::vector<Layer*> Layers;
		unsigned int LayerInsertIndex = 0;
	};
}