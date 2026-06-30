#include "Eigen/Dense"
namespace TiGER
{
	template <int dim, typename DerivedM>
	void matrix_to_list(
		const Eigen::DenseBase<DerivedM>& M,
		std::vector<std::array<typename DerivedM::Scalar, dim>>& V)
	{
		using namespace std;
		V.resize(M.rows(), std::array<typename DerivedM::Scalar, dim>());
		// loop over rows
		for (int i = 0; i < M.rows(); i++)
		{
			// loop over cols
			for (int j = 0; j < dim; j++)
			{
				V[i][j] = M(i, j);
			}
		}
	}

	template <int dim, typename DerivedM>
	void matrix_to_list(
		const Eigen::DenseBase<DerivedM>& M,
		std::vector<typename DerivedM::Scalar>& V)
	{
		using namespace std;
		if (dim != M.cols()) {
			throw std::runtime_error("out of range");
		}
		V.resize(M.rows()*dim);
		// loop over rows
		for (int i = 0; i < M.rows(); i++)
		{
			for (int j = 0; j < dim; j++) {
				// loop over cols
				V[i*dim+j] = M(i, j);
			}
		}
	}
}
