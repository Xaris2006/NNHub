#include <iostream>

#include "NeuronNetwork.h"

#include <thread>
#include <fstream>
#include <vector>
#include <cstdint>
#include <intrin.h>
#include <chrono>
#include <array>

double g_trainingAmount;

void load_mnist(const std::string& filename, NN::Mat& X, NN::Mat& Y)
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

int jmain()
{
	NN::NeuronNetwork::NetworkSpecification spec;
	spec.input = 28 * 28;
	spec.LayerOptions.emplace_back(12, NN::Functions::Sigmoid);
	spec.LayerOptions.emplace_back(10, NN::Functions::Sigmoid);
	spec.LayerOptions.emplace_back(10, NN::Functions::Sigmoid);
	spec.outputPath = "output2.nn";
	spec.costF = NN::Functions::SC;

	NN::NeuronNetwork cnn(spec);

	cnn.Read(spec.outputPath);

	NN::Mat X_test;
	NN::Mat Y_test;

	load_mnist("mnist_test\\mnist_test.csv", X_test, Y_test);

	int suc = 0;

	for (int i = 0; i < X_test.n_rows - 1; i++)
	{
		auto output = cnn.GetOutput(X_test.row(i).t());
		
		int b = 0;
		int B = 0;
		for (int j = 1; j < output.n_rows; j++)
		{
			if (output.at(b) < output.at(j))
				b = j;

			if (Y_test.at(i, B) < Y_test.at(i, j))
				B = j;
		}

		if (b = B)
			suc++;

		std::cout << '\r' << suc * 100 / (i + 1) << "%";
	}

	return 0;
}

int hmain()
{
	NN::NeuronNetwork::NetworkSpecification spec;
	spec.input = 28*28;
	spec.LayerOptions.emplace_back(12, NN::Functions::Sigmoid);
	spec.LayerOptions.emplace_back(10, NN::Functions::Sigmoid);
	spec.LayerOptions.emplace_back(10, NN::Functions::Sigmoid);
	spec.outputPath = "output2.nn";
	spec.costF = NN::Functions::SC;

	NN::NeuronNetwork cnn(spec);

	NN::Mat X_train, X_test;
	NN::Mat Y_train, Y_test;

	load_mnist("mnist_train\\mnist_train.csv", X_train, Y_train);
	//load_mnist("mnist_test\\mnist_test.csv", X_test, Y_test);

	const int step = (X_train.n_rows - 1) / 2000;
	g_trainingAmount = step;

	auto start = std::chrono::high_resolution_clock::now();

	std::cout << '\n';

	for (int i = 0; i < 2000; i++)
	{
		double persent = 0;

		for (int j = 0; j < X_train.n_rows - 1; j++)
		{
			bool optimize = (0 == (j + step + 1) % step);
			double cost = cnn.Train(X_train.row(j).t(), Y_train.row(j).t(), optimize);
			if (optimize)
				persent += cost;
		}

		std::cout << "Cost: " << persent/2000 << "\r";
	}

	auto end = std::chrono::high_resolution_clock::now();
	
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "Elapsed time: " << duration.count() << " ms" << std::endl;

	std::cout << '\n' << std::endl;
	cnn.WriteOutput();

	return 0;
}