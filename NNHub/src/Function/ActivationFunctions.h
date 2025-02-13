#pragma once

#include "Types.h"

namespace NN::Functions
{
	enum ActivationFunction
	{
		Sigmoid = 0,
		ReLU,
		LeakyReLU,
		Tanh,
		Empty
	};

	//-------

	Vec _Sigmoid(const Vec& v);

	Vec _ReLU(const Vec& v);

	Vec _LeakyReLU(const Vec& v);

	Vec _Tanh(const Vec& v);

	Vec _Empty(const Vec& v);

	inline Vec(*CallActivationFunction[]) (const Vec&) =
	{
		_Sigmoid, _ReLU, _LeakyReLU, _Tanh, _Empty
	};

	//-------

	Vec D_Sigmoid(const Vec& v);

	Vec D_ReLU(const Vec& v);

	Vec D_LeakyReLU(const Vec& v);

	Vec D_Tanh(const Vec& v);

	Vec D_Empty(const Vec& v);

	inline Vec(*CallActivationFunctionDerivative[]) (const Vec&) =
	{
		D_Sigmoid, D_ReLU, D_LeakyReLU, D_Tanh, D_Empty
	};

	//-------
}