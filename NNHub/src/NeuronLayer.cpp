#include "NeuronLayer.h"

#include <random>
#include <iostream>
#include <thread>
#include <future>

static const double g_LearningRate = 0.01;

namespace NN
{
	Layer::Layer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data)
		:m_NeuronsW(AmountOfNeurons, AmountOfPreviusLineInput, arma::fill::randn), m_NeuronsB(AmountOfNeurons, arma::fill::randn),
		m_Activations(AmountOfNeurons, arma::fill::zeros), m_Z(AmountOfNeurons, arma::fill::zeros),
		m_D_NeuronsW(AmountOfNeurons, AmountOfPreviusLineInput, arma::fill::zeros), m_D_NeuronsB(AmountOfNeurons, arma::fill::zeros),
		m_ActivationF(activationF), m_banchSize(banchSize)
	{
		if (data)
		{
			for (int n = 0; n < AmountOfNeurons; n++)
			{
				for (int i = 0; i < AmountOfPreviusLineInput; i++)
					m_NeuronsW(n, i) = data[i + n * (AmountOfPreviusLineInput + 1)];
				m_NeuronsB(n) = data[AmountOfPreviusLineInput + 1 + n * (AmountOfPreviusLineInput + 1)];
			}
		}
	}

	void Layer::SetValues(double* data)
	{
		if (data)
		{
			for (int n = 0; n < m_NeuronsW.n_rows; n++)
			{
				for (int i = 0; i < m_NeuronsW.n_cols; i++)
					m_NeuronsW(n, i) = data[i + n * (m_NeuronsW.n_cols + 1)];
				m_NeuronsB(n) = data[m_NeuronsW.n_cols + 1 + n * (m_NeuronsW.n_cols + 1)];
			}
		}
	}

	void Layer::GetValues(Mat& v) const
	{
		v = arma::join_rows(m_NeuronsW, m_NeuronsB);
	}

	void Layer::Optimizer()
	{
		const RealType clip_value = 1.5;
		m_D_NeuronsW = arma::clamp(m_D_NeuronsW, -clip_value, clip_value);
		m_D_NeuronsB = arma::clamp(m_D_NeuronsB, -clip_value, clip_value);

		m_NeuronsW -= (m_D_NeuronsW * (g_LearningRate / m_banchSize));
		m_D_NeuronsW.fill(0);
		
		m_NeuronsB -= (m_D_NeuronsB * (g_LearningRate / m_banchSize));
		m_D_NeuronsB.fill(0);
	}

	FirstLayer::FirstLayer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data)
		:Layer(AmountOfNeurons, AmountOfPreviusLineInput, activationF, banchSize, data)
	{

	}

	void FirstLayer::Init(HiddenLayer* nextLine)
	{
		m_nextLine = nextLine;
	}

	Vec FirstLayer::Test(const Vec& input) const
	{
		return m_nextLine->Test(Functions::CallActivationFunction[m_ActivationF](m_NeuronsW * input + m_NeuronsB));
	}

	void FirstLayer::UpdateActivations(const Vec& Input)
	{
		m_Input = Input;

		m_Z = (m_NeuronsW * Input + m_NeuronsB);
		m_Activations = Functions::CallActivationFunction[m_ActivationF](m_Z);

		m_nextLine->UpdateActivations(m_Activations);
	}

	void FirstLayer::GradientDescent(const Vec& NextLayer_dC_da)
	{
		Vec d = Functions::CallActivationFunctionDerivative[m_ActivationF](m_Z);

		Mat dC_da = (d * m_Input.t());
		dC_da.each_col() %= NextLayer_dC_da;

		m_D_NeuronsW += (dC_da);
		m_D_NeuronsB += (d % NextLayer_dC_da);
	}

	HiddenLayer::HiddenLayer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data)
		:Layer(AmountOfNeurons, AmountOfPreviusLineInput, activationF, banchSize, data)
	{

	}

	void HiddenLayer::Init(Layer* prevLine, Layer* nextLine)
	{
		m_nextLine = nextLine;
		m_prevLine = prevLine;
	}

	Vec HiddenLayer::Test(const Vec& input) const
	{
		return m_nextLine->Test(Functions::CallActivationFunction[m_ActivationF](m_NeuronsW * input + m_NeuronsB));
	}

	void HiddenLayer::UpdateActivations(const Vec& Input)
	{
		m_Z = (m_NeuronsW * Input + m_NeuronsB);
		m_Activations = Functions::CallActivationFunction[m_ActivationF](m_Z);

		m_nextLine->UpdateActivations(m_Activations);
	}

	void HiddenLayer::GradientDescent(const Vec& NextLayer_dC_da)
	{
		Vec d = Functions::CallActivationFunctionDerivative[m_ActivationF](m_Z);

		Mat dC_da = (d * m_prevLine->GetActivations().t());
		dC_da.each_col() %= NextLayer_dC_da;

		m_D_NeuronsW += (dC_da);
		m_D_NeuronsB += (d % NextLayer_dC_da);

		Mat next = m_NeuronsW;
		next.each_col() %= d;

		m_prevLine->GradientDescent(next.t() * NextLayer_dC_da);
	}

	LastLayer::LastLayer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data)
		:Layer(AmountOfNeurons, AmountOfPreviusLineInput, activationF, banchSize, data)
	{

	}

	void LastLayer::Init(HiddenLayer* prevLine, Functions::CostFunction CostF)
	{
		m_prevLine = prevLine;
		m_costF = CostF;
	}

	Vec LastLayer::Test(const Vec& input) const
	{
		return Functions::CallActivationFunction[m_ActivationF](m_NeuronsW * input + m_NeuronsB);
	}

	void LastLayer::UpdateActivations(const Vec& Input)
	{
		m_Z = (m_NeuronsW * Input + m_NeuronsB);
		m_Activations = Functions::CallActivationFunction[m_ActivationF](m_Z);
	}

	void LastLayer::GradientDescent(const Vec& Output)
	{
		Vec da_dz = Functions::CallActivationFunctionDerivative[m_ActivationF](m_Z);
		Vec dC_da = Functions::CallCostFunctionDerivative[m_costF](m_Activations, Output);
		Mat da_dw = (da_dz * m_prevLine->GetActivations().t());
		da_dw.each_col() %= dC_da;

		m_D_NeuronsW += da_dw;
		m_D_NeuronsB += (da_dz % dC_da);

		Mat next = m_NeuronsW;
		next.each_col() %= da_dz;

		m_prevLine->GradientDescent(next.t() * dC_da);
	}
}