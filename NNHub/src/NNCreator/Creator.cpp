#include "Creator.h"

#include <fstream>

static void loadMnist(const std::string& filename, NN::Mat& X, NN::Mat& Y)
{
	NN::Mat data;

	if (!data.load(filename, arma::csv_ascii)) {
		std::cerr << "Error loading " << filename << std::endl;
		exit(1);
	}
	
	NN::Vec y = data.col(0);

	X = data.cols(1, data.n_cols - 1);

	X /= 255.0;

	Y.set_size(data.n_rows, 10);
	Y.fill(arma::fill::zeros);

	for (int i = 0; i < data.n_rows; ++i)
	{
		Y.at(i, y[i]) = 1;
	}
}

static Creator* s_creator = nullptr;

void Creator::Init()
{
	s_creator = new	Creator();
}

void Creator::ShutDown()
{
	delete s_creator;
	s_creator = nullptr;
}

void Creator::OnUpdate()
{
	if (s_creator->m_buildThread && s_creator->m_threadEnded)
	{
		s_creator->EndBuild();
	}
}

Creator& Creator::Get()
{
	return *s_creator;
}

Creator::~Creator()
{
	EndBuild();
}

void Creator::Clear()
{
	if (m_buildThread)
		return;

	m_cost = 0;
	m_testPercentage = 0;
}

void Creator::StartBuild()
{
	if (m_buildThread)
		return;

	m_controrThread = 0;
	m_status = Status::Building;
	m_threadEnded = false;

	m_buildThread = new std::thread(
		[&]()
		{
			m_Spec.input = 28 * 28;
			m_Spec.outputPath = m_name;

			NN::Mat X_train, X_test;
			NN::Mat Y_train, Y_test;

			loadMnist("mnist_train\\mnist_train.csv", X_train, Y_train);
			loadMnist("mnist_test\\mnist_test.csv", X_test, Y_test);

			const int step = std::min((int)X_train.n_rows - 1, m_banchSize);
			m_Spec.banchSize = step;

			NN::NeuronNetwork cnn(m_Spec);

			const int MaxTraining = m_repeat;

			for (int i = 0; i < MaxTraining; i++)
			{
				float persent = 0;

				for (int j = 0; j < X_train.n_rows - 1; j++)
				{
					if (m_controrThread)
					{
						if (m_controrThread == 2)
							return;
						else
						{
							while (m_controrThread)
							{
								if (m_controrThread == 2)
									return;
								std::this_thread::sleep_for(std::chrono::milliseconds(200));
							}
						}
					}

					bool optimize = (0 == (j + step + 1) % step);
					double cost = cnn.Train(X_train.row(j).t(), Y_train.row(j).t(), optimize);
					if (optimize)
						persent += cost;
				}
				
				m_cost = persent / (X_train.n_rows / step);

				std::cout << "Cost: " << m_cost << "\r";

				m_percentage = std::min(100.0f * (i / (float)MaxTraining), 99.0f);
				
				{
					int suc = 0;

					for (int j = 0; j < X_test.n_rows - 1; j++)
					{
						auto output = cnn.GetOutput(X_test.row(j).t());

						int b = 0;
						int B = 0;
						for (int k = 1; k < output.n_rows; k++)
						{
							if (output.at(b) < output.at(k))
								b = k;

							if (Y_test.at(j, B) < Y_test.at(j, k))
								B = k;
						}

						if (b == B)
							suc++;
					}

					m_testPercentage = suc / (float)(X_test.n_rows - 1) * 100.0f;
				}
			}

			if (m_controrThread)
			{
				if (m_controrThread == 2)
					return;
				else
				{
					while (m_controrThread)
					{
						if (m_controrThread == 2)
							return;
						std::this_thread::sleep_for(std::chrono::milliseconds(200));
					}
				}
			}

			cnn.WriteOutput();

			m_percentage = 100;
			m_threadEnded = true;
		}

	);
}

void Creator::PauseBuild()
{
	if (!m_buildThread)
		return;

	m_controrThread = 1;
	m_status = Status::Paused;
}

void Creator::ResumeBuild()
{
	if (!m_buildThread)
		return;

	m_controrThread = 0;
	m_status = Status::Building;
}

void Creator::EndBuild()
{
	if (!m_buildThread)
		return;

	m_controrThread = 2;
	m_status = Status::Nothing;

	m_buildThread->join();
	delete m_buildThread;
	m_buildThread = nullptr;
	
	m_percentage = 0;

	Clear();
}

Creator::Status Creator::GetStatus() const
{
	return m_status;
}

float Creator::GetPercentage() const
{
	return m_percentage;
}

float Creator::GetTestPercentage() const
{
	return m_testPercentage;
}

float Creator::GetCost() const
{
	return m_cost;
}

void Creator::SetName(const std::string& name)
{
	if (m_status != Nothing)
		return;
	m_name = name;
}

std::string& Creator::GetName()
{
	return m_name;
}

void Creator::SetRepeatness(int repeat)
{
	if (m_status != Nothing)
		return;
	m_repeat = repeat;
}

int& Creator::GetRepeatness()
{
	return m_repeat;
}

void Creator::SetBanchSize(int banchSize)
{
	if (m_status != Nothing)
		return;
	m_banchSize = banchSize;
}

int& Creator::GetBanchSize()
{
	return m_banchSize;
}

NN::NeuronNetwork::NetworkSpecification& Creator::GetSpecification()
{
	return m_Spec;
}
