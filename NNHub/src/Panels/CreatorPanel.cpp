#include "CreatorPanel.h"

#include "../NNCreator/Creator.h"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "Walnut/UI/UI.h"

#include "implot.h"

#include <fstream>
#include <iostream>

struct ScrollingBuffer {
	int MaxSize;
	int Offset;
	ImVector<ImVec2> Data;
	ScrollingBuffer(int max_size = 2000) {
		MaxSize = max_size;
		Offset = 0;
		Data.reserve(MaxSize);

		Data.push_back(ImVec2(0, 0));
		Offset = (Offset + 1) % MaxSize;
	}
	void AddPoint(float x, float y) {
		if (Data.size() < MaxSize)
			Data.push_back(ImVec2(x, y));
		else {
			Data[Offset] = ImVec2(x, y);
			Offset = (Offset + 1) % MaxSize;
		}
	}
	void Erase() {
		if (Data.size() > 0) {
			Data.shrink(0);
			Offset = 0;
		}
	}
};

static ScrollingBuffer sdata3, sdataD, sdataT;

namespace Panels
{
	void CreatorPanel::OnAttach()
	{
		m_PlayIcon = std::make_shared<Walnut::Image>("Icons\\PlayButton.png");
		m_StopIcon = std::make_shared<Walnut::Image>("Icons\\StopButton.png");
		m_ResumeIcon = std::make_shared<Walnut::Image>("Icons\\ResumeButton.png");
		m_PauseIcon = std::make_shared<Walnut::Image>("Icons\\PauseButton.png");

		Creator::Get().SetName("MyCNN");
	}

	void  CreatorPanel::OnDetach()
	{

	}

	void CreatorPanel::OnUIRender()
	{
		Creator::OnUpdate();

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
		{
			ImGui::BeginDisabled(Creator::Get().GetStatus() != Creator::Nothing);

			if (ImGui::IsKeyPressed(ImGuiKey_A))
			{
				
			}

			//ImGui::BeginDisabled(Creator::Get().GetSize() == 0);

			if (ImGui::IsKeyPressed(ImGuiKey_B))
			{
				Creator::Get().StartBuild();
			}

			//ImGui::EndDisabled();
			ImGui::EndDisabled();
		}

		ImGui::Begin("Build Area");

		ImGui::BeginDisabled(Creator::Get().GetStatus() == Creator::Nothing);

		static float t = 0;

		if (Creator::Get().GetStatus() == Creator::Nothing)
			t = 0;

		if (Creator::Get().GetStatus() == Creator::Building)
		{
			t += ImGui::GetIO().DeltaTime;
			sdata3.AddPoint(t, Creator::Get().GetCost());
			static float last = Creator::Get().GetCost();
			static float lastT = t;
			float dC_dt = (Creator::Get().GetCost() - last) / (t - lastT);
			sdataD.AddPoint(t, dC_dt);
			last = Creator::Get().GetCost();
			lastT = t;
			sdataT.AddPoint(t, Creator::Get().GetTestPercentage());

		}

		static float history = 10.0f;
		ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");

		if (ImPlot::BeginSubplots("##CostPlots", 1, 2, ImVec2(-1, 400)))
		{
			static ImPlotAxisFlags flags = ImPlotAxisFlags_LockMin;

			std::string costValue = "Cost = " + std::to_string(Creator::Get().GetCost()) + "##CPID";
			if (ImPlot::BeginPlot(costValue.c_str(), ImVec2(-1, 400)))
			{
				ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
				ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 2);

				ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

				//ImPlot::PlotLine("##Cost", &sdata3.Data[0].x, &sdata3.Data[0].y, sdata3.Data.size(), 0, sdata3.Offset, 2 * sizeof(float));
				ImPlot::PlotShaded("##Cost", &sdata3.Data[0].x, &sdata3.Data[0].y, sdata3.Data.size(), -INFINITY, 0, sdata3.Offset, 2 * sizeof(float));

				ImPlot::EndPlot();
			}

			std::string costValueD = "Cost Derivative to time";
			if (ImPlot::BeginPlot(costValueD.c_str(), ImVec2(-1, 400)))
			{
				//ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
				ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2, ImGuiCond_Once);

				//ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

				ImPlot::PlotLine("##CostD", &sdataD.Data[0].x, &sdataD.Data[0].y, sdataD.Data.size(), 0, sdataD.Offset, 2 * sizeof(float));
				//ImPlot::PlotShaded("##Cost", &sdataD.Data[0].x, &sdataD.Data[0].y, sdataD.Data.size(), -INFINITY, 0, sdataD.Offset, 2 * sizeof(float));

				ImPlot::EndPlot();
			}

			ImPlot::EndSubplots();
		}

		//float tp = (int)(Creator::Get().GetTestPercentage() * 10) / 10.0f;

		std::string tPercentage = "Test Percentage = " + std::to_string(Creator::Get().GetTestPercentage());
		if (ImPlot::BeginPlot(tPercentage.c_str(), ImVec2(-1, 400)))
		{
			//ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
			ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

			//ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

			ImPlot::PlotLine("##TestP", &sdataT.Data[0].x, &sdataT.Data[0].y, sdataT.Data.size(), 0, sdataT.Offset, 2 * sizeof(float));
			//ImPlot::PlotShaded("##Cost", &sdataD.Data[0].x, &sdataD.Data[0].y, sdataD.Data.size(), -INFINITY, 0, sdataD.Offset, 2 * sizeof(float));

			ImPlot::EndPlot();
		}

		ImGui::EndDisabled();

		ImGui::Separator();

		if (Creator::Get().GetStatus() == Creator::Nothing)
		{
			//ImGui::BeginDisabled(files.empty());

			if (ImGui::ImageButton((ImTextureID)m_PlayIcon->GetRendererID(), { 22, 22 })
				&& Creator::Get().GetSpecification().LayerOptions.size() >= 3)
			{
				sdata3.Erase();
				sdataD.Erase();
				sdataT.Erase();

				Creator::Get().Creator::StartBuild();
			}

			//ImGui::EndDisabled();
		}
		else
		{
			if (Creator::Get().GetStatus() == Creator::Paused)
			{
				if (ImGui::ImageButton((ImTextureID)m_ResumeIcon->GetRendererID(), { 22, 22 }))
				{
					Creator::Get().Creator::ResumeBuild();
				}
			}
			else
			{
				if (ImGui::ImageButton((ImTextureID)m_PauseIcon->GetRendererID(), { 22, 22 }))
				{
					Creator::Get().Creator::PauseBuild();
				}
			}

		}

		ImGui::BeginDisabled(Creator::Get().GetStatus() == Creator::Nothing);

		ImGui::SameLine();

		if (ImGui::ImageButton((ImTextureID)m_StopIcon->GetRendererID(), { 22, 22 }))
		{
			Creator::Get().Creator::EndBuild();

			sdata3.Erase();
			sdata3.AddPoint(0, 0);
			sdataD.Erase();
			sdataD.AddPoint(0, 0);
			sdataT.Erase();
			sdataT.AddPoint(0, 0);
		}

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.58f, 0.97f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.58f, 0.97f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.58f, 0.97f, 0.6f));

		ImGui::Button((std::to_string((int)Creator::Get().GetPercentage()) + '%').c_str(), { 55, ImGui::GetFrameHeight() });

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		auto ycursor = ImGui::GetCursorPosY();
		auto xcursor = ImGui::GetCursorPosX();

		float availx = ImGui::GetContentRegionAvail().x;
		ImGui::Button("##end", ImVec2(availx, 0));

		ImGui::SetCursorPosY(ycursor);
		ImGui::SetCursorPosX(xcursor);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.353f, 0.314f, 0.0118f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.353f, 0.314f, 0.0118f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.353f, 0.314f, 0.0118f, 1.0f));

		ImGui::Button("##bar", ImVec2(availx * Creator::Get().GetPercentage() / 100, 0));

		ImGui::PopStyleColor(3);

		ImGui::EndDisabled();

		ImGui::End();

		ImGui::Begin("Settings");

		ImGui::BeginDisabled(Creator::Get().GetStatus() != Creator::Nothing);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.58f, 0.97f, 1.0f));

		ImGui::Text(("Creation Path: " + std::filesystem::current_path().string() + '\\').c_str());

		ImGui::PopStyleColor();

		ImGui::SameLine();

		auto& spec = Creator::Get().GetSpecification();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
		
		ImGui::SetNextItemWidth(ImGui::CalcTextSize("12345678911131517192123252729313335373941").x);
		ImGui::InputText("Output Name", &Creator::Get().GetName());

		ImGui::InputInt("Training Repeatness", &Creator::Get().GetRepeatness(), 100, 1000);
		ImGui::InputInt("Banch Size", &Creator::Get().GetBanchSize(), 10, 100);

		ImGui::Separator();

		//ImGui::InputInt("Input", (int*)&spec.input, 10, 100);

		ImGui::Separator();

		if (ImGui::Button("Add Layer"))
			spec.LayerOptions.emplace_back(std::pair(10, NN::Functions::Sigmoid));

		static int removeLayer = -1;

		for (int i = 0; i < spec.LayerOptions.size(); ++i)
		{
			ImGui::PushID(i);
			
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("12345678911131517192123").x);
			ImGui::Combo("##Activation Function", (int*)&spec.LayerOptions[i].second, NN::Functions::ActivationFunctionName, 5);
			
			ImGui::SameLine();

			std::string layerID = "Layer " + std::to_string(i + 1);
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("12345678911131517192123").x);
			ImGui::InputInt(layerID.c_str(), (int*)&spec.LayerOptions[i].first, 10, 100);

			ImGui::SameLine();

			if(ImGui::Button("Remove"))
				removeLayer = i;

			ImGui::PopID();
		}

		if (removeLayer != -1)
		{
			spec.LayerOptions.erase(spec.LayerOptions.begin() + removeLayer);
			removeLayer = -1;
		}

		ImGui::Separator();

		ImGui::SetNextItemWidth(ImGui::CalcTextSize("12345678911131517192123").x);
		ImGui::Combo("Cost Function", (int*)&spec.costF, NN::Functions::CostFunctionName, 5);

		ImGui::EndDisabled();

		ImGui::End();

	}


}