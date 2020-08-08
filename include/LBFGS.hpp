﻿#include<math.h>
#include<vector>
#include"VectorAlgebra.hpp"

namespace pnc{

inline int getFloatingIndex(int i,int cursor,int bufferSize) {
    return (cursor - 1 - i + bufferSize) % bufferSize;
}

template<int buffer_size,typename data_type, int dimension>
class LBFGS
{
private:
    using Vector = Vector<dimension,data_type>;
    data_type hessian_estimate = 0;
    unsigned int _cursor = 0;
    unsigned int _activeBufferSize=0;

    Vector _s[buffer_size + 1]; // saves all the stuff column wise, fortran style
    Vector _y[buffer_size + 1]; // one element extra used in update
    data_type _alpha[buffer_size];
    data_type _rho[buffer_size];

public:
    inline constexpr LBFGS()
    {
        _activeBufferSize = 0;
    }
    
    inline constexpr void solve(
        const Vector& gradient,
        Vector& outputDirection)
    {
        if (_activeBufferSize == 0) 
        {
            outputDirection = -gradient;
            return;
        }

        outputDirection = gradient*1.0;
        for (int absolute_i = 0; absolute_i < _activeBufferSize; ++absolute_i)
        {
            unsigned int i = getFloatingIndex(absolute_i, _cursor, buffer_size);

            _rho[i] = 1/(_s[i]*_y[i]);
            _alpha[i] = _rho[i]*(_s[i]*outputDirection);

            outputDirection = outputDirection -(_alpha[i]*_y[i]);
        }

        outputDirection = outputDirection * hessian_estimate;

        data_type beta;
        for (int absolute_i = _activeBufferSize-1; absolute_i > -1; --absolute_i)
        {
            unsigned int i = getFloatingIndex(absolute_i, _cursor, buffer_size);

            beta = _rho[i]*(_y[i]*outputDirection);
            outputDirection = outputDirection + (_alpha[i]-beta)*_s[i];
        }

        outputDirection = -outputDirection;
    }
    
    template<typename TVectorS,typename TVectorY>
    inline constexpr bool CheckIfValidUpdate(
        const Vector& gradientCurrentLocations,
        const TVectorS& potentialS,
        const TVectorY& potentialY)
    {
        // Theoretical condition:
        // update if (y^Ts)/||s || ^ 2 > epsilon * || grad(x) ||
        data_type value = (potentialS*potentialY)
            / (potentialS*potentialS);

        return value > sqrt(Norm(gradientCurrentLocations))*(1e-12);
    }

    // Update the hessian estimate
    // -> returns true if hessian was adjusted
    // -> returns false if carefull update avoided an hessian update
    inline constexpr bool updateHessian(
        const Vector& currentLocation, 
        const Vector& currentGradient, 
        const Vector& newLocation, 
        const Vector& newGradient)
    {
        auto potentialS = newLocation - currentLocation;
        auto potentialY = newGradient - currentGradient;

        if (this->CheckIfValidUpdate(currentGradient, potentialS, potentialY))
        {
            _s[_cursor] = potentialS;
            _y[_cursor] = potentialY;
    
            hessian_estimate = (potentialS*potentialY)
                / (potentialY*potentialY);

            if (_activeBufferSize < buffer_size)
            {
                _activeBufferSize++;
            }
            _cursor = (_cursor + 1) % buffer_size;
            return true;
        }
        return false;
    }
};
}
