#include "Eigen/Dense"
#include <vector>
#include <stdexcept>
#ifndef _LIST_TOMATRIX_H_
#define _LIST_TOMATRIX_H_

namespace TiGER
{
    // Convert a list of std::array to an Eigen::Matrix
    template <int dim, typename Scalar, typename DerivedM>
    void list_to_matrix(
        const std::vector<std::array<Scalar, dim>>& V,
        Eigen::PlainObjectBase<DerivedM>& M)
    {
        if(V.size()==0||dim<=0)
            throw std::runtime_error("invalid dim");
        M.resize(V.size(),dim);
        // loop over rows
        for (int i = 0; i < V.size(); i++)
        {
            // loop over cols
            for (int j = 0; j < dim; j++)
            {
                M(i, j) = V[i][j];
            }
        }
    }

    // Convert a flat vector to an Eigen::Matrix
    template <int dim, typename Scalar, typename DerivedM>
    void list_to_matrix(
        const std::vector<Scalar>& V,
        Eigen::PlainObjectBase<DerivedM>& M)
    {
        int numRows = V.size() / dim;
         M.resize(V.size(),dim);
        // loop over rows
        for (int i = 0; i < numRows; i++)
        {
            // loop over cols
            for (int j = 0; j < dim; j++)
            {
                M(i, j) = V[i*dim + j];
            }
        }
    }
}

#endif //_LIST_TOMATRIX_H_