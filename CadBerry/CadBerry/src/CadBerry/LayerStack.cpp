#include "cdbpch.h"
#include "LayerStack.h"
#include "Log.h"

namespace CDB
{
	LayerStack::LayerStack()
	{
		
	}

	LayerStack::~LayerStack()
	{
		CDB_EditorTrace("LayerStack deleted");
		for (Layer* layer : Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		layer->OnAttach();
		Layers.emplace(Layers.begin() + LayerInsertIndex, layer);
		LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		overlay->OnAttach();
		Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		layer->OnDetach();
		auto it = std::find(Layers.begin(), Layers.end(), layer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		overlay->OnDetach();
		auto it = std::find(Layers.begin(), Layers.end(), overlay);
		if (it != Layers.end())
			Layers.erase(it);
	}
}