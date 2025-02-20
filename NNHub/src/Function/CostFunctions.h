#pragma once

#include "Types.h"

namespace NN::Functions
{
	enum CostFunction
	{
		SC = 0,
		MSC,
		LogisticC,
		MAE,
		HingeLoss
	};
	
	inline const char* const CostFunctionName[] =
	{
		"Sc",
		"MSC",
		"LogisticC",
		"MAE",
		"Hinge Loss"

	};

	//-------

	double _SC(const Vec& Output, const Vec& exOutput);

	double _MSC(const Vec& Output, const Vec& exOutput);

	double _LogisticC(const Vec& Output, const Vec& exOutput);

	double _MAE(const Vec& Output, const Vec& exOutput);

	double _HingeLoss(const Vec& Output, const Vec& exOutput);
	
	inline double(*CallCostFunction[]) (const Vec&, const Vec&) =
	{
		_SC, _MSC, _LogisticC, _MAE, _HingeLoss
	};

	//-------

	Vec D_SC(const Vec& Output, const Vec& exOutput);

	Vec D_MSC(const Vec& Output, const Vec& exOutput);

	Vec D_LogisticC(const Vec& Output, const Vec& exOutput);

	Vec D_MAE(const Vec& Output, const Vec& exOutput);

	Vec D_HingeLoss(const Vec& Output, const Vec& exOutput);

	inline Vec (*CallCostFunctionDerivative[]) (const Vec&, const Vec&) =
	{
		D_SC, D_MSC, D_LogisticC, D_MAE, D_HingeLoss
	};

	//-------
}