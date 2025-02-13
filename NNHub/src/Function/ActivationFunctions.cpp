#include "ActivationFunctions.h"

namespace NN::Functions
{
	//-------

	Vec _Sigmoid(const Vec& v)
	{
		return 1.0 / (1.0 + arma::exp(-v));
	}

	Vec _ReLU(const Vec& v)
	{
		return arma::clamp(v, 0.0, arma::datum::inf);
	}

	Vec _LeakyReLU(const Vec& v)
	{
		const RealType alpha = 0.01;
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
		return s % (1.0 - s);
	}

	Vec D_ReLU(const Vec& v)
	{
		return arma::conv_to<Vec>::from(v > 0);
	}

	Vec D_LeakyReLU(const Vec& v)
	{
		const RealType alpha = 0.01;
		return arma::conv_to<Vec>::from(v > 0) + alpha * arma::conv_to<Vec>::from(v <= 0);
	}

	Vec D_Tanh(const Vec& v)
	{
		return 1.0 - arma::square(arma::tanh(v));
	}

	Vec D_Empty(const Vec& v)
	{
		return Vec(v.n_rows, arma::fill::ones);
	}

	//-------
}