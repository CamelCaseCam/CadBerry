#define FMT_HEADER_ONLY

#include "CadBerry/Utils/memory.h"
#include "CadBerry.h"
#include "CadBerry/Project/Project.h"

#include "Complement.h"
#include "Simulation.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ImPlot.h"

#include "CadBerry/Threading/ThreadPool.h"

#include <glm/ext/matrix_transform.hpp>

#include <nfd.h>

#include <filesystem>
#include <stdlib.h>
#include <sys/stat.h>

CDB::Application* CDBApp;

float TotalTimeForThreadPool;
Simulation* SimulationForThreadPool;
std::string OutputPathForThreadPool;

#ifdef CDB_DEVELOPER_ENABLE
//Good example for doing your own rendering
class __declspec(dllexport) RenderingTests : public CDB::Viewport
{
public:
	RenderingTests() : CDB::Viewport("Rendering Tests") {};
	~RenderingTests() override {};

	enum class RenderTest
	{
		NONE,
		TRIANGLE,
		TEXTURE,
		MOVING_TEXTURE,
	};
	RenderTest TestNumber = RenderTest::NONE;
	CDB::scoped_ptr<CDB::RenderTarget> Target;
	CDB::scoped_ptr<CDB::VertexArray> TriangleVerts;
	CDB::scoped_ptr<CDB::VertexArray> SquareVerts;

	CDB::scoped_ptr<CDB::Shader> TriangleShader;
	CDB::scoped_ptr<CDB::Shader> SquareShader;
	CDB::scoped_ptr<CDB::Shader> SquareTransformShader;

	CDB::scoped_ptr<CDB::Texture> SquareTexture;

	virtual void Update(float dt) override
	{

	}

	virtual void Draw() override    //Not working
	{
		
	}

	/*
	Suuuuuper not professional to be ranting in my comments, but not professional is my jam!
	Today (2022-02-14) I found out that Youtube Vanced has been shut down. I hate google. I only downloaded vanced because of all the ways youtube 
	destroyed their user experience (removing dislikes, 2x 15sec unskipable adds), and now this makes me want to use youtube even less. I hope 
	Google gets broken up
	*/
	virtual void GUIDraw() override
	{
		CDB::Renderer::BeginScene(Target.raw());
		CDB::RenderCommand::SetClearColour({ 0.0, 0.0, 0.0, 1.0 });
		CDB::RenderCommand::Clear();

		switch (TestNumber)
		{
		case RenderingTests::RenderTest::NONE:
			break;
		case RenderingTests::RenderTest::TRIANGLE:
			TriangleShader->Bind();
			CDB::Renderer::Submit(this->TriangleVerts.raw());
			break;
		case RenderingTests::RenderTest::TEXTURE:
			SquareShader->Bind();
			SquareTexture->Bind();
			CDB::Renderer::Submit(this->SquareVerts.raw());
			break;
		case RenderingTests::RenderTest::MOVING_TEXTURE:
			SquareTexture->Bind();
			glm::mat4 Trans = glm::mat4(1.0f);
			Trans = glm::rotate(Trans, ((float)ImGui::GetTime()) * glm::radians(50.0f), glm::vec3(0.0, 0.0, 1.0));
			CDB::Renderer::Submit(this->SquareVerts.raw(), SquareTransformShader.raw(), Trans);
			break;
		default:
			break;
		}

		CDB::Renderer::EndScene();

		ImGui::GetCurrentWindow()->Flags |= ImGuiWindowFlags_MenuBar;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Tests"))
			{
				if (ImGui::MenuItem("Triangle"))
				{
					CDB_BuildInfo("Worked");
					TestNumber = RenderTest::TRIANGLE;
				}
				if (ImGui::MenuItem("Textured Square"))
				{
					TestNumber = RenderTest::TEXTURE;
				}
				if (ImGui::MenuItem("Moving Textured Square"))
				{
					TestNumber = RenderTest::MOVING_TEXTURE;
					auto pos = (glm::mat4(1.0f) * glm::vec4(-1.0, 1.0, 0.0, 1.0));
					CDB_EditorInfo("[{0}, {1}, {2}, {3}]", pos.x, pos.y, pos.z, pos.w);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		Target->Draw({ 400, 400 }, {1.0, 1.0});
	}

	virtual void Start() override
	{
		using namespace CDB;    //You probably shouldn't do this, but I'm the *Lord Of CadBerry*, so it's okay here
		this->Target = RenderTarget::Create();
		this->TriangleVerts = VertexArray::Create();
		this->SquareVerts = VertexArray::Create();

		float TriangleVertices[5 * 3] = {    //Initialize vertices
			0.0f, 1.0f,			1.0f, 0.0f, 0.0f,
			1.0f, -1.0f,		0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f,		0.0f, 0.0f, 1.0f
		};

		VertexBuffer* TriangleBuffer = VertexBuffer::Create(sizeof(TriangleVertices), TriangleVertices);

		{
			BufferLayout layout = {
				{ShaderDataType::Float2, "pos"},
				{ShaderDataType::Float3, "colour"}
			};
			TriangleBuffer->SetLayout(layout);
		}

		uint32_t TriangleIndices[3] = { 0, 1, 2 };
		IndexBuffer* TriangleIndexBuffer = IndexBuffer::Create(sizeof(TriangleIndices), TriangleIndices);

		this->TriangleVerts->AddVertexBuffer(TriangleBuffer);
		this->TriangleVerts->SetIndexBuffer(TriangleIndexBuffer);
		TriangleIndexBuffer = nullptr;    //If I'm in release mode without this, the compiler merges TriangleIndexBuffer and SquareIndexBuffer

		this->TriangleShader = Shader::Create(
			R"(#version 330 core
layout(location = 0) in vec2 InPos;
layout(location = 1) in vec3 InColour;
out vec4 v_Colour;

void main()
{
	gl_Position = vec4(InPos, 0.0, 1.0);
	v_Colour = vec4(InColour, 1.0);
})",
R"(#version 330 core
layout(location = 0) out vec4 colour;
in vec4 v_Colour;
void main()
{
	colour = v_Colour;
})");


		//Textured square stuff
		float SquareVerts[4 * 4] = {
		-1.0f, 1.0f,		0.0f, 1.0f,
		1.0f, 1.0f,			1.0f, 1.0f,
		1.0f, -1.0f,		1.0f, 0.0f,
		-1.0f, -1.0f,		0.0f, 0.0f
		};

		VertexBuffer* SquareBuffer = VertexBuffer::Create(sizeof(SquareVerts), SquareVerts);

		{
			BufferLayout layout = {
				{ShaderDataType::Float2, "pos"},
				{ShaderDataType::Float2, "uv"},
			};

			SquareBuffer->SetLayout(layout);
		}

		unsigned int SquareIndices[6] = { 0, 1, 2, 0, 2, 3 };

		IndexBuffer* SquareIndexBuffer = IndexBuffer::Create(sizeof(SquareIndices), SquareIndices);
		this->SquareVerts->AddVertexBuffer(SquareBuffer);
		this->SquareVerts->SetIndexBuffer(SquareIndexBuffer);

		//Now load the texture
		this->SquareTexture = Texture::Create(CDB::Application::Get().PathToEXE + "/Tests/235227762_152116843723682_5504231419480605688_n.jpg");

		this->SquareShader = Shader::Create(
R"(#version 330 core
layout(location = 0) in vec2 InPos;
layout(location = 1) in vec2 InUV;
out vec2 v_UV;

void main()
{
	gl_Position = vec4(InPos, 0.0, 1.0);
	v_UV = InUV;
})",

R"(#version 330 core
layout(location = 0) out vec4 colour;
in vec2 v_UV;
uniform sampler2D Texture;

void main()
{
	colour = texture(Texture, v_UV);
})");

		this->SquareTransformShader = Shader::Create(
			R"(#version 330 core
layout(location = 0) in vec2 InPos;
layout(location = 1) in vec2 InUV;
out vec2 v_UV;

uniform mat4 u_PVMatrix;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_Transform * vec4(InPos, 0.0, 1.0);
	v_UV = InUV;
})",

R"(#version 330 core
layout(location = 0) out vec4 colour;
in vec2 v_UV;
uniform sampler2D Texture;

void main()
{
	colour = texture(Texture, v_UV);
})");
	}

	virtual void OnClose() override {}
};
#endif //CDB_DEVELOPER_ENABLE

class __declspec(dllexport) Modelling : public CDB::Viewport
{
public:
	Modelling() : CDB::Viewport("Modelling") {};
	~Modelling() override {};
	Simulation CurrentSimulation;
	
	virtual void Update(float dt) override
	{

	}

	std::string SimSrc;
	float TotalTime = 0.0f;
	float Timestep = 0.1f;
	bool RunCSV = false;
	std::string OutputPath = "";
	float GraphSizes[4] = { 0.0, 10.0, 0.0, 10.0 };

	static void* RunSimulation(void* Params) { SimulationForThreadPool->Run(TotalTimeForThreadPool); return nullptr; }
	static void* RunSimulationCSV(void* Params) { SimulationForThreadPool->RunToCSV(TotalTimeForThreadPool, OutputPathForThreadPool); return nullptr; }
	virtual void GUIDraw() override
	{
		ImGui::InputTextMultiline("Simulation source code", &SimSrc);
		ImGui::InputFloat("Simulation timestep", &Timestep);
		ImGui::InputFloat("Total time of simulation", &TotalTime);
		ImGui::InputFloat4("Graph window size", &GraphSizes[0]);
		ImGui::Checkbox("Export to CSV", &RunCSV);

		if (RunCSV)
		{
			if (ImGui::Button("Select an output path"))
			{
				nfdchar_t* Output = nullptr;
				nfdresult_t result = NFD_SaveDialog("csv", NULL, &Output);
				if (result == NFD_OKAY)
					OutputPath = Output;
			}
		}

		if (ImGui::Button("Run simulation"))
		{
			CurrentSimulation = Simulation(SimSrc);
			CurrentSimulation.SetTimestep(Timestep);
			CurrentSimulation.Reset();
			TotalTimeForThreadPool = TotalTime;
			SimulationForThreadPool = &CurrentSimulation;

			if (!RunCSV)
			{
				//Run the simulation asynchronously
				CDB::ThreadPool::Get()->AddStandardTask(RunSimulation, nullptr);
			}
			else
			{
				OutputPathForThreadPool = OutputPath + ".csv";
				CDB::ThreadPool::Get()->AddStandardTask(RunSimulationCSV, nullptr);
			}
		}
		ImPlot::SetNextAxesLimits(GraphSizes[0], GraphSizes[1], GraphSizes[2], GraphSizes[3], ImPlotCond_Always);
		CurrentSimulation.Draw();
	}

	virtual void Start() override
	{
		CurrentSimulation.Reset();
	}

	virtual void OnClose() override {}
};

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
		//RenderTarget example:
		//CDB::Renderer::BeginScene(Target.raw());

		//CDB::RenderCommand::SetClearColour({ 1.0, 0.0, 0.0, 1.0 });
		//CDB::RenderCommand::Clear();

		//CDB::Renderer::Submit(vertexArray.raw());

		//CDB::Renderer::EndScene();

		//if (ImgSize.x == 0 && ImgSize.y == 0)
		//{
		//	ImgSize = ImGui::GetWindowSize();
		//}

		//Target->Draw(ImgSize, ImVec2(1, 1));


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

	/*CDB::scoped_ptr<CDB::VertexArray> vertexArray;
	CDB::scoped_ptr<CDB::RenderTarget> Target;
	ImVec2 ImgSize = ImVec2(0, 0);*/
	virtual void Start() override 
	{
		//Part of RenderTarget example:
		/*Target = CDB::RenderTarget::Create();
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
		vertexArray->SetIndexBuffer(indexBuffer);*/
	}
	virtual void OnClose() override {}
};

enum class ViewportType
{
	DNAEditor,
	Modelling,
#ifdef CDB_DEVELOPER_ENABLE
	RenderingTests,
#endif // CDB_DEVELOPER_ENABLE
};

std::unordered_map<std::string, ViewportType> Name2Viewport({
	{ "DNA Editor", ViewportType::DNAEditor },
	{ "Modelling", ViewportType::Modelling },
#ifdef CDB_DEVELOPER_ENABLE
	{ "Rendering Tests", ViewportType::RenderingTests } ,
#endif // CDB_DEVELOPER_ENABLE
});

class __declspec(dllexport) CoreModule : public CDB::Module
{
public:
	CoreModule() : CDB::Module("Core CadBerry module") {}
	~CoreModule() override {}

	UseImGui
	
#ifdef CDB_DEVELOPER_ENABLE
	std::string Viewports[3] = { "DNA Editor", "Modelling", "Rendering Tests" };
#else
	std::string Viewports[2] = { "DNA Editor", "Modelling" };
#endif // CDB_DEVELOPER_ENABLE
	virtual std::string* GetViewportNames() override
	{
#ifdef CDB_DEVELOPER_ENABLE
		NumViewports = 3;
#else
		NumViewports = 2;
#endif // CDB_DEVELOPER_ENABLE
		return Viewports;
	}

	virtual CDB::Viewport* CreateViewport(std::string viewport) override
	{
		switch (Name2Viewport[viewport])
		{
		case ViewportType::DNAEditor:
			return new DNAEditor();
		case ViewportType::Modelling:
			return new Modelling();
#ifdef CDB_DEVELOPER_ENABLE
		case ViewportType::RenderingTests:
			return new RenderingTests();
#endif // CDB_DEVELOPER_ENABLE
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