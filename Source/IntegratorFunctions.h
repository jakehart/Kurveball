// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once

#include <typeinfo>

#include "IntegratorFunctionType.h"

namespace CurveLib
{
	template<typename ScalarT, typename VectorT>
	class IntegratorFunctions
	{
	public:
		using ScalarType = ScalarT;
		using VectorType = VectorT;

		// All of these integration functions are signed, in the sense that negative y values produce
		// negative outputs. This is needed for backwards motion.
		static ScalarType LeftRiemann(const VectorType& previousSample, const VectorType& currentSample);

		static ScalarType RightRiemann(const VectorType& previousSample, const VectorType& currentSample);

		static ScalarType Trapezoidal(const VectorType& previousSample, const VectorType& currentSample);
		// TODO: static_assert sigs using function type ^
	};
}

#include "IntegratorFunctions.inl"