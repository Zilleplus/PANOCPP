﻿#pragma once

#include <panoc/vectorAlgebra/vector.hpp>
#include <panoc/entities.hpp>
#include <panoc/LipschitzEstimator.hpp>
#include <panoc/vectorAlgebra/tostring.hpp>
#include <panoc/VectorAlgebra.hpp>
#include <iostream>
#include <limits>

namespace pnc {

	// Contains a proximal gradient step:
	// -. Does not own the vector's itself !
	template<typename TVec>
	struct ProximalGradientStep {
		ProximalGradientStep(
			Location<TVec>& c,
			Location<TVec>& p) :
			current(c),
			proximal(p) {}

		Location<TVec>& current;
		Location<TVec>& proximal;

	};

	template<
		typename TCostFunction,
		typename TProximalOperator
	>
		class ProximalCalculator {
		public:
			using Estimator = LipschitzEstimator;
			TProximalOperator& prox_;

			template<typename TValue>
			struct Config {
				/// The linesearch condition should be :
				// fNew > f - df.DotProduct(df) 
				//     + (1 ) / (2 * new_location.Gamma) * (direction.DotProduct(direction))
				//
				// In reality it is:
				// fNew > f - df.DotProduct(df)
				//     + (1 - safetyValueLineSearch) / (2 * new_location.Gamma) * directionSquaredNorm
				//     + 1e-6 * f;
				static constexpr TValue safety_value_line_search = 0.05;
				// The linesearch parameter gamma can never be smaller then this
				static constexpr TValue min_gamma_value = std::numeric_limits<TValue>::epsilon() * 10;
			};

			TCostFunction& cost_function_;

			ProximalCalculator(TCostFunction& cost_function, TProximalOperator& prox) :
				cost_function_(cost_function),
				prox_(prox) {}

			template<
				typename TVector,
				typename data_type = typename TVector::data_type
			>
				void Calculate(
					const Location<TVector>& current,
					Location<TVector>& proximal)
			{
				TakeProxStep(
					current,
					current.gamma,
					proximal);

				while (!LinesearchCondition(current, proximal, Config<typename TVector::data_type>::safety_value_line_search) &&
					(proximal.gamma / data_type{ 2 } > Config<typename TVector::data_type>::min_gamma_value))
				{
					TakeProxStep(
						current,
						proximal.gamma / data_type{ 2 },
						proximal);
				}
			}

		private:
			// use the value of current_location to calculate a
			// new proximal gradient step.
			template<
				typename TVector,
				typename data_type = typename TVector::data_type
			>
				void TakeProxStep(
					const Location<TVector>& current,
					const data_type gamma,
					Location<TVector>& proximal) const
			{
				proximal.gamma = gamma;
				proximal.location = prox_(current.location - gamma * current.gradient);
				proximal.cost = cost_function_(
					proximal.location,
					proximal.gradient);
			}

			template<
				typename TVector,
				typename data_type = typename TVector::data_type
			>
				bool LinesearchCondition(
					const Location<TVector>& current,
					const Location<TVector>& proximal,
					const data_type safety_value_line_search)  const
			{
				const auto direction_squared_norm = (current.location - proximal.location)
					* (current.location - proximal.location);

				const auto f = current.cost;
				const auto& df = current.gradient;
				const auto f_new = proximal.cost;
				//const auto& df_new = proximal.gradient;

				return f_new > f - df * df
					+ (1 - safety_value_line_search) / (2 * proximal.gamma)
					* direction_squared_norm
					+ 1e-6 * f;
			}
	};

}
