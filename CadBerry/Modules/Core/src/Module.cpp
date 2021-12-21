#define FMT_HEADER_ONLY

#include "CadBerry/Utils/memory.h"
#include "CadBerry.h"
#include "CadBerry/Project/Project.h"

#include "Complement.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <filesystem>
#include <stdlib.h>
#include <sys/stat.h>

CDB::Application* CDBApp;

class __declspec(dllexport) DNAEditor : public CDB::Viewport
{
public:
	DNAEditor() : CDB::Viewport("DNA Editor") {};
	~DNAEditor() override {};

	int DNAOperation = 0;
	const std::vector<const char*> DNAOperations = {
		"Reverse Complement",
		"Complement",
		"Reverse strand"
	};
	std::string DNA;
	std::string Output;

	virtual void GUIDraw() override
	{
		CDB::Renderer::BeginScene(Target.raw());

		CDB::RenderCommand::SetClearColour({ 1.0, 0.0, 0.0, 1.0 });
		CDB::RenderCommand::Clear();

		CDB::Renderer::Submit(vertexArray.raw());

		CDB::Renderer::EndScene();

		if (ImgSize.x == 0 && ImgSize.y == 0)
		{
			ImgSize = ImGui::GetWindowSize();
		}

		Target->Draw(ImgSize, ImVec2(1, 1));
		ImGui::ListBox("DNA transformation", &DNAOperation, DNAOperations.data(), DNAOperations.size());
		ImGui::InputText("DNA", &DNA);
		if (ImGui::Button("Transform"))
		{
			switch (DNAOperation)
			{
			case 0:
				Core::GetReverseComplement(DNA, Output);
				break;
			case 1:
				Core::GetComplement(DNA, Output);
				break;
			case 2:
				Core::Reverse(DNA, Output);
				break;
			default:
				break;
			}
		}
		ImGui::Text(Output.c_str());
		if (ImGui::Button("Copy output"))
		{
			ImGui::SetClipboardText(Output.c_str());
		}
	}

	CDB::scoped_ptr<CDB::VertexArray> vertexArray;
	CDB::scoped_ptr<CDB::RenderTarget> Target;
	ImVec2 ImgSize = ImVec2(0, 0);
	virtual void Start() override 
	{
		Target = CDB::RenderTarget::Create();
		vertexArray = CDB::VertexArray::Create();

		float vertices[3 * 7] = {
			0.1f, 0.6f, 0.1f,     0.5f, 0.2f, 0.8f, 1.0f,
			-0.4f, -0.4f, 0.1f,   0.2f, 0.2f, 0.8f,	1.0f,
			0.6f, -0.4f, 0.1f,    0.8f, 0.8f, 0.4f,	1.0f,
		};

		CDB::VertexBuffer* buffer = CDB::VertexBuffer::Create(sizeof(vertices), vertices);

		{
			CDB::BufferLayout layout = {
				{CDB::ShaderDataType::Float3, "pos"},
				{CDB::ShaderDataType::Float4, "colour"},
			};

			buffer->SetLayout(layout);
		}

		uint32_t indices[3] = { 0, 1, 2 };
		CDB::IndexBuffer* indexBuffer = CDB::IndexBuffer::Create(3, indices);

		vertexArray->AddVertexBuffer(buffer);
		vertexArray->SetIndexBuffer(indexBuffer);
	}
	virtual void OnClose() override {}
};

enum class ViewportType
{
	DNAEditor
};

std::unordered_map<std::string, ViewportType> Name2Viewport({
	{ "DNA Editor", ViewportType::DNAEditor }
});

class __declspec(dllexport) CoreModule : public CDB::Module
{
public:
	CoreModule() : CDB::Module("Core CadBerry module") {}
	~CoreModule() override {}

	UseImGui
	
	std::string Viewports[1] = { "DNA Editor" };
	virtual std::string* GetViewportNames() override
	{
		NumViewports = 1;
		return Viewports;
	}

	virtual CDB::Viewport* CreateViewport(std::string viewport) override
	{
		switch (Name2Viewport[viewport])
		{
		case ViewportType::DNAEditor:
			return new DNAEditor();
		default:
			return nullptr;
		}
	}
};

//Expose the module to CadBerry
extern "C"
{
	__declspec(dllexport) CDB::Module* __stdcall GetModule()
	{
		return new CoreModule();
	}
}