#pragma once

#include <vector>
#include <filesystem>
#include <thread>

class Creator
{
public:
	enum Status
	{
		Nothing = 0,
		Building,
		Paused
	};

public:
	static void Init();
	static void ShutDown();
	static void OnUpdate();
	static Creator& Get();

public:
	void Clear();
	
	void StartBuild();
	void PauseBuild();
	void ResumeBuild();
	void EndBuild();

	Status GetStatus() const;
	float GetPercentage() const;
	float GetCost() const;
	float GetTestPercentage() const;

	void SetName(const std::string& name);
	std::string& GetName();

	void SetRepeatness(int repeat);
	int& GetRepeatness();
	
	void SetBanchSize(int banchSize);
	int& GetBanchSize();

private:
	Creator() = default;
	~Creator();

private:
	Status m_status = Nothing;
	float m_percentage = 0;
	float m_testPercentage = 0;
	float m_cost = 0;

	std::string m_name = "";
	int m_repeat = 100;
	int m_banchSize = 10;

	std::thread* m_buildThread = nullptr;
	int m_controrThread = 2;
	bool m_threadEnded = true;
};
