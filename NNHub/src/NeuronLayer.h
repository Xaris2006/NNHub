#pragma once

#include "Function/CostFunctions.h"
#include "Function/ActivationFunctions.h"

namespace NN
{
	class Layer
	{
	public:
		Layer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data = nullptr);

		void SetValues(double* data);

		void GetValues(Mat& v) const;

		virtual Vec Test(const Vec& input) const = 0;

		virtual void UpdateActivations(const Vec& Input) = 0;
		virtual void GradientDescent(const Vec& NextLayer_dC_da) = 0;
		virtual void Optimizer();

		virtual Vec& GetActivations() = 0;
		virtual Mat& GetNeurons() = 0;

	protected:
		Mat m_NeuronsW;
		Vec m_NeuronsB;
		Vec m_Activations;
		Vec m_Z;

		Mat m_D_NeuronsW;
		Vec m_D_NeuronsB;

		Functions::ActivationFunction m_ActivationF;
		int m_banchSize;
	};

	class FirstLayer;
	class HiddenLayer;
	class LastLayer;

	class FirstLayer : public Layer
	{
	public:
		FirstLayer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data = nullptr);

		void Init(HiddenLayer* nextLine);

		virtual Vec Test(const Vec& input) const override;
		
		virtual void UpdateActivations(const Vec& Input) override;

	protected:
		virtual void GradientDescent(const Vec& NextLayer_dC_da) override;

		virtual Vec& GetActivations() override { return m_Activations; }
		virtual Mat& GetNeurons() override { return m_NeuronsW; }

	private:
		HiddenLayer* m_nextLine = nullptr;
		Vec m_Input;

		friend class HiddenLayer;
		friend class LastLayer;
	};

	class HiddenLayer : public Layer
	{
	public:
		HiddenLayer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data = nullptr);

		void Init(Layer* prevLine, Layer* nextLine);

	protected:
		virtual Vec Test(const Vec& input) const override;

		virtual void UpdateActivations(const Vec& Input) override;
		virtual void GradientDescent(const Vec& NextLayer_dC_da) override;

		virtual Vec& GetActivations() override { return m_Activations; }
		virtual Mat& GetNeurons() override { return m_NeuronsW; }

	private:
		Layer* m_nextLine = nullptr;
		Layer* m_prevLine = nullptr;

		friend class FirstLayer;
		friend class LastLayer;
	};

	class LastLayer : public Layer
	{
	public:
		LastLayer(unsigned int AmountOfNeurons, unsigned int AmountOfPreviusLineInput, Functions::ActivationFunction activationF, int banchSize, double* data = nullptr);

		void Init(HiddenLayer* prevLine, Functions::CostFunction CostF);

		virtual void GradientDescent(const Vec& Output) override;

	protected:
		virtual Vec Test(const Vec& input) const override;

		virtual void UpdateActivations(const Vec& Input) override;

		virtual Vec& GetActivations() override { return m_Activations; }
		virtual Mat& GetNeurons() override { return m_NeuronsW; }

	private:
		HiddenLayer* m_prevLine = nullptr;
		Functions::CostFunction m_costF;

		friend class FirstLayer;
		friend class HiddenLayer;
	};
}