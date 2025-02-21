#include "ActivationFunctions.h"

namespace NN::Functions
{
	using namespace CuBlas;
	//-------

	Vec _Sigmoid(const Vec& v)
	{
		return 1.0f / (1.0f + arma::exp(-v));
	}

	Vec _ReLU(const Vec& v)
	{
		return arma::clamp(v, 0.0f, arma::fdatum::inf);
	}

	Vec _LeakyReLU(const Vec& v)
	{
		const RealType alpha = 0.01f;
		return arma::max(alpha * v, v);
	}

	Vec _Tanh(const Vec& v)
	{
		return arma::tanh(v);
	}

	Vec _Empty(const Vec& v)
	{
		return v;
	}

	//-------

	Vec D_Sigmoid(const Vec& v)
	{
		Vec s = _Sigmoid(v);
		return s % (1.0f - s);
	}

	Vec D_ReLU(const Vec& v)
	{
		return arma::conv_to<Vec>::from(v > 0);
	}

	Vec D_LeakyReLU(const Vec& v)
	{
		const RealType alpha = 0.01f;
		return arma::conv_to<Vec>::from(v > 0) + alpha * arma::conv_to<Vec>::from(v <= 0);
	}

	Vec D_Tanh(const Vec& v)
	{
		return 1.0f - arma::square(arma::tanh(v));
	}

	Vec D_Empty(const Vec& v)
	{
		return Vec(v.n_rows, arma::fill::ones);
	}

	//-------
}