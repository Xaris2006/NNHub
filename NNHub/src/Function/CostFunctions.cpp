#include "CostFunctions.h"

namespace NN::Functions
{
	//-------

	double _SC(const Vec& Output, const Vec& exOutput)
	{
		return arma::sum(arma::square(Output - exOutput));
	}

	double _MSC(const Vec& Output, const Vec& exOutput)
	{
		return arma::mean(arma::square(Output - exOutput));
	}

	double _LogisticC(const Vec& Output, const Vec& exOutput)
	{
		return -arma::mean(Output % arma::log(exOutput) + (1 - Output) % arma::log(1 - exOutput));
	}

	double _MAE(const Vec& Output, const Vec& exOutput)
	{
		return arma::mean(arma::abs(Output - exOutput));
	}

	double _HingeLoss(const Vec& Output, const Vec& exOutput)
	{
		return arma::mean(arma::max(arma::zeros<Vec>(Output.n_elem), 1 - Output % exOutput));
	}

	//-------

	Vec D_SC(const Vec& Output, const Vec& exOutput)
	{
		return 2.0 * (Output - exOutput);
	}

	Vec D_MSC(const Vec& Output, const Vec& exOutput)
	{
		return 2.0 * (Output - exOutput) / Output.n_elem;
	}

	Vec D_LogisticC(const Vec& Output, const Vec& exOutput)
	{
		return -arma::log(exOutput / (1.0 - exOutput)) / Output.n_elem;
	}

	Vec D_MAE(const Vec& Output, const Vec& exOutput)
	{
		return arma::sign(Output - exOutput) / Output.n_elem;
	}

	Vec D_HingeLoss(const Vec& Output, const Vec& exOutput)
	{
		Vec grad = arma::zeros<Vec>(Output.n_elem);
		for (arma::uword i = 0; i < Output.n_elem; ++i) {
			if (1.0 - Output[i] * exOutput[i] > 0)
				grad[i] = -exOutput[i] / Output.n_elem;
			else
				grad[i] = 0;
		}
		return Vec(grad);
	}

	//-------
}