#pragma once

#include "NeuronLayer.h"

#include <filesystem>

namespace NN
{

	class NeuronNetwork
	{
	public:
		struct NetworkSpecification
		{
			std::filesystem::path outputPath;
			
			unsigned int input;
			std::vector<std::pair<unsigned int, Functions::ActivationFunction>> LayerOptions;
			
			Functions::CostFunction costF;
			//size_t branchSize;
			RealType learningRate;
			int banchSize;
		};

	public:
		NeuronNetwork(NetworkSpecification spec);
		NeuronNetwork(const std::filesystem::path& infilePath);

		void WriteOutput() const;
		bool Test(const Vec& input, const Vec& expectedOuput) const;

		double GetCost(const Vec& input, const Vec& expectedOuput) const;

		double Train(const Vec& input, const Vec& expectedOuput, bool optimize = false);

		Vec GetOutput(const Vec& input) const;

		void Read(const std::filesystem::path& infilePath);

	private:
		NetworkSpecification m_spec;
		NN::FirstLayer* m_firstLayer;
		std::vector<NN::HiddenLayer*> m_hiddenLayers;
		NN::LastLayer* m_lastLayer;
	};

}
