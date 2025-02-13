#include "NeuronNetwork.h"

#include <fstream>
#include <iostream>

namespace NN
{
	NeuronNetwork::NeuronNetwork(NetworkSpecification spec)
		:m_spec(spec)
	{
		if (spec.input == 0 || spec.LayerOptions.size() < 3)
			return;

		m_hiddenLayers.reserve(spec.LayerOptions.size() - 2);

		m_firstLayer = new NN::FirstLayer(spec.LayerOptions[0].first, spec.input, spec.LayerOptions[0].second, spec.banchSize);
		m_lastLayer = new NN::LastLayer(spec.LayerOptions[spec.LayerOptions.size() - 1].first, spec.LayerOptions[spec.LayerOptions.size() - 2].first, spec.LayerOptions[spec.LayerOptions.size() - 1].second, spec.banchSize);

		for (int i = 1; i < spec.LayerOptions.size() - 1; i++)
			m_hiddenLayers.emplace_back(new HiddenLayer(spec.LayerOptions[i].first, spec.LayerOptions[i - 1].first, spec.LayerOptions[i].second, spec.banchSize));

		m_firstLayer->Init(m_hiddenLayers[0]);

		if(m_hiddenLayers.size() == 1)
			m_hiddenLayers[0]->Init(m_firstLayer, m_lastLayer);
		else if (m_hiddenLayers.size() == 2)
		{
			m_hiddenLayers[0]->Init(m_firstLayer, m_hiddenLayers[1]);
			m_hiddenLayers[1]->Init(m_hiddenLayers[0], m_lastLayer);
		}
		else
		{
			m_hiddenLayers[0]->Init(m_firstLayer, m_hiddenLayers[1]);
			for (int i = 1; i < m_hiddenLayers.size() - 1; i++)
				m_hiddenLayers[i]->Init(m_hiddenLayers[i - 1], m_hiddenLayers[i + 1]);
			m_hiddenLayers[m_hiddenLayers.size() - 1]->Init(m_hiddenLayers[m_hiddenLayers.size() - 2], m_lastLayer);
		}

		m_lastLayer->Init(m_hiddenLayers[m_hiddenLayers.size() - 1], m_spec.costF);
	}

	NeuronNetwork::NeuronNetwork(const std::filesystem::path& infilePath)
	{
		//m_spec.outputPath = infilePath;
		//
		//std::ifstream infile(infilePath, std::ios::binary);
		//
		//infile.seekg(0, std::ios_base::end);
		//std::streampos maxSize = infile.tellg();
		//infile.seekg(0, std::ios_base::beg);
		//
		//std::vector<double> v;
		//v.reserve(maxSize / 8);
		//v.resize(maxSize / 8);
		//
		//infile.read((char*)v.data(), maxSize);
		//infile.close();
		//
		//if (v.empty())
		//	return;
		//
		//size_t power = v[0];
		//m_spec.input = v[1];
		//
		//for (int i = 2; i < INT32_MAX; i++)
		//{
		//	if (v[i] == 0)
		//		break;
		//
		//	m_spec.LayerOptions.emplace_back(v[i]);
		//}

		//m_Lines.reserve(m_spec.neuronPerLine.size());
		//
		//size_t index = 0;
		//
		//m_Lines.emplace_back(m_spec.neuronPerLine[0], m_spec.input, &v[index]);
		//
		//index += (m_spec.input + 1) * m_spec.neuronPerLine[0];
		//
		//for (int i = 1; i < m_spec.neuronPerLine.size(); i++)
		//{
		//	m_Lines.emplace_back(m_spec.neuronPerLine[i], m_spec.neuronPerLine[i - 1], &v[index]);
		//	index += (m_spec.neuronPerLine[i - 1] + 1) * m_spec.neuronPerLine[i];
		//}
		//
		//m_Lines[0].Init(nullptr, &m_Lines[1]);
		//
		//for (int i = 1; i < m_Lines.size() - 1; i++)
		//	m_Lines[i].Init(&m_Lines[i - 1], &m_Lines[i + 1]);
		//
		//m_Lines[m_Lines.size() - 1].Init(&m_Lines[m_Lines.size() - 2], nullptr);
	}

	void NeuronNetwork::WriteOutput() const
	{
		std::ofstream output(m_spec.outputPath);

		//output << m_spec.input << '\n';
		//
		//for (int i = 0; i < m_spec.neuronPerLine.size(); i++)
		//	output << m_spec.neuronPerLine[i] << '\n';

		Mat val;

		m_firstLayer->GetValues(val);

		for (int i = 0; i < val.n_rows; i++)
		{
			for (int j = 0; j < val.n_cols; j++)
				output << val.at(i, j) << ' ';
		}

		output << '\n';

		for (auto& h : m_hiddenLayers)
		{
			h->GetValues(val);
			for (int i = 0; i < val.n_rows; i++)
			{
				for (int j = 0; j < val.n_cols; j++)
					output << val.at(i, j) << ' ';
			}

			output << '\n';
		}

		m_lastLayer->GetValues(val);

		for (int i = 0; i < val.n_rows; i++)
		{
			for (int j = 0; j < val.n_cols; j++)
				output << val.at(i, j) << ' ';
		}
		
		output.close();
	}

	bool NeuronNetwork::Test(const Vec& input, const Vec& expectedOuput) const
	{
		std::cout << "\nStart Test\n-------------------------\n";

		Vec output;
			
		output = m_firstLayer->Test(input);

		for (int i = 0; i < output.size(); i++)
		{
			std::cout << "output[ " << i << " ] : " << output[i] << " : -- : " << expectedOuput[i] << '\n';
		}
		std::cout << "Cost: " << Functions::CallCostFunction[m_spec.costF](output, expectedOuput) << '\n';
		std::cout << "-------------------------\nEnd Test\n";

		return true;
	}

	double NeuronNetwork::GetCost(const Vec& input, const Vec& expectedOuput) const
	{
		return Functions::CallCostFunction[m_spec.costF](m_firstLayer->Test(input), expectedOuput);
	}

	double NeuronNetwork::Train(const Vec& input, const Vec& expectedOuput, bool optimize)
	{
		m_firstLayer->UpdateActivations(input);
		m_lastLayer->GradientDescent(expectedOuput);

		if (optimize)
		{
			m_firstLayer->Optimizer();
			m_lastLayer->Optimizer();
			for (auto& hidden : m_hiddenLayers)
				hidden->Optimizer();
			
			return Functions::CallCostFunction[m_spec.costF](m_firstLayer->Test(input), expectedOuput);
		}

		return -1;
	}

	Vec NeuronNetwork::GetOutput(const Vec& input) const
	{
		return m_firstLayer->Test(input);
	}

	void NeuronNetwork::Read(const std::filesystem::path& infilePath)
	{
		std::ifstream infile(infilePath);

		//output << m_spec.input << '\n';
		//
		//for (int i = 0; i < m_spec.neuronPerLine.size(); i++)
		//	output << m_spec.neuronPerLine[i] << '\n';

		std::vector<double> data;

		double in = 0;

		for (int i = 0; i < m_spec.LayerOptions[0].first; i++)
		{
			for (int j = 0; j < m_spec.input + 1; j++)
			{
				infile >> in;
				data.emplace_back(in);
			}
		}

		m_firstLayer->SetValues(data.data());

		for (int k = 1; k < m_spec.LayerOptions.size(); k++)
		{
			data.clear();

			for (int i = 0; i < m_spec.LayerOptions[k].first; i++)
			{
				for (int j = 0; j < m_spec.LayerOptions[k - 1].first + 1; j++)
				{
					infile >> in;
					data.emplace_back(in);
				}
			}

			if (k != (m_spec.LayerOptions.size() - 1))
				m_hiddenLayers[k - 1]->SetValues(data.data());
		}

		m_lastLayer->SetValues(data.data());

		infile.close();
	}
}