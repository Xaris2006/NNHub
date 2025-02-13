#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/UI/UI.h"

#include "Windows/WindowsUtils.h"

#include <filesystem>
#include <iostream>
#include <array>
#include <fstream>

#include "GLFW/glfw3.h"

#include "Panels/CreatorPanel.h"
#include "NNCreator/Creator.h"

#include "implot.h"

std::u8string g_AppDirectory;
Walnut::ApplicationSpecification g_spec;
bool g_AlreadyOpenedModalOpen = false;

static std::vector<std::string> s_arg;

class ChessLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override
	{
		Creator::Init();
		
		m_CreatorPanel = std::make_unique<Panels::CreatorPanel>();
		m_CreatorPanel->OnAttach();

		glfwMaximizeWindow(Walnut::Application::Get().GetWindowHandle());
		glfwFocusWindow(Walnut::Application::Get().GetWindowHandle());
		
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
	}

	virtual void OnDetach() override
	{
		m_CreatorPanel->OnDetach();
		
		Creator::ShutDown();
	}

	virtual void OnUIRender() override
	{	
		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
		{
			if (ImGui::IsKeyPressed(ImGuiKey_N))
			{
				New();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_O))
			{
				Open();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_S))
			{
				if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
					SaveAs();
				else
					Save();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_F))
			{
				FlipBoard();
			}

			if (ImGui::IsKeyPressed(ImGuiKey_E))
			{
				OpenEditor();
			}
		}

		m_CreatorPanel->OnUIRender();

		UI_DrawAboutModal();
		AlreadyOpenedModal();

		//ImPlot::ShowDemoWindow();
		//ImGui::ShowDemoWindow();
		//ImGui::ShowMetricsWindow();
	}

	void UI_DrawAboutModal()
	{
		if (!m_AboutModalOpen)
			return;

		ImGui::OpenPopup("About");
		m_AboutModalOpen = ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		if (m_AboutModalOpen)
		{
			auto image = Walnut::Application::Get().GetApplicationIcon();
			ImGui::Image((ImTextureID)image->GetRendererID(), { 48, 48 });

			ImGui::SameLine();
			Walnut::UI::ShiftCursorX(20.0f);

			ImGui::BeginGroup();
			ImGui::Text("LightSource is a Chess GUI");
			ImGui::Text("by C.Betsakos");
			ImGui::EndGroup();

			if (Walnut::UI::ButtonCentered("Close"))
			{
				m_AboutModalOpen = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void AlreadyOpenedModal()
	{
		if (!g_AlreadyOpenedModalOpen)
			return;
		ImGui::OpenPopup("Error! File Is Already Opened");

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		
		g_AlreadyOpenedModalOpen = ImGui::BeginPopupModal("Error! File Is Already Opened", 0);
		
		if (g_AlreadyOpenedModalOpen)
		{
			ImGui::TextWrapped("The file that you are trying to open is already opened in a different LightSource Window!");

			ImGui::NewLine();

			ImGui::PushID("in2");

			ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Close").x - 18);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7, 0.1, 0.1, 0.65));
			if (ImGui::Button("Close"))
			{
				g_AlreadyOpenedModalOpen = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor();

			ImGui::PopID();

			ImGui::EndPopup();
		}
	}

	void OpenEditor()
	{
		
	}

	void FlipBoard()
	{
		
	}

	void ShowAboutModal()
	{
		m_AboutModalOpen = true;
	}

	void New()
	{
		
	}

	void Open()
	{
		std::string filepath = Windows::Utils::OpenFile("(*.pgn)\0*.pgn\0");
		if (!filepath.empty())
		{
			
		}
	}

	void SaveAs()
	{
		std::string filepath = Windows::Utils::SaveFile("(*.pgn)\0*.pgn\0");
		if (!filepath.empty())
		{
			
		}
	}

	void Save()
	{
		if ("New Game")
		{
			SaveAs();
		}
		else
			;
	}


private:
	bool m_AboutModalOpen = false;

	std::unique_ptr<Panels::CreatorPanel> m_CreatorPanel;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	g_spec.Name = "Neuron NetWork Hub";
	g_spec.CustomTitlebar = true;
	//g_spec.AppIconPath = "Resources\\LightSource\\lsb.png";
	//g_spec.IconPath = "Resources\\LightSource\\ls.png";
	//g_spec.HoveredIconPath = "Resources\\LightSource\\lsOn.png";
	g_spec.FuncIconPressed = []()
		{

		};

	//fix arg
	s_arg.emplace_back(argv[0]);
	for (int i = 1; i < argc; i++)
	{
		if (std::filesystem::path(s_arg[s_arg.size() - 1]).has_extension())
			s_arg.emplace_back(argv[i]);
		else
		{
			s_arg[s_arg.size() - 1] += ' ';
			s_arg[s_arg.size() - 1] += argv[i];
		}
	}

	g_AppDirectory = std::filesystem::path(s_arg[0]).parent_path().u8string();

#if defined(WL_DIST)
	std::filesystem::current_path(g_AppDirectory);
#endif

	Walnut::Application* app = new Walnut::Application(g_spec, 237 + 60);
	std::shared_ptr<ChessLayer> chessLayer = std::make_shared<ChessLayer>();
	app->PushLayer(chessLayer);
	app->SetMenubarCallback([app, chessLayer]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctr+N"))
			{
				chessLayer->New();
			}
			if (ImGui::MenuItem("Open", "Ctr+O"))
			{
				chessLayer->Open();
			}
			if (ImGui::MenuItem("Save", "Ctr+S"))
			{
				chessLayer->Save();
			}
			if (ImGui::MenuItem("Save As", "Ctr+Shift+S"))
			{
				chessLayer->SaveAs();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Alt+F4"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				chessLayer->ShowAboutModal();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}