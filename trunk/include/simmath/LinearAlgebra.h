#ifndef SimTK_LINEAR_ALGEBRA_H_
#define SimTK_LINEAR_ALGEBRA_H_

/* Portions copyright (c) 2007 Stanford University and Jack Middleton.
 * Contributors:
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS, COPYRIGHT HOLDERS, OR CONTRIBUTORS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** @file
 * This is the header file that user code should include to pick up the 
 * SimTK Simmath numerical differentiation tools.
 */


/* Shared libraries are messy in Visual Studio. We have to distinguish three
 * cases:
 *   (1) this header is being used to build the simmath shared library (dllexport)
 *   (2) this header is being used by a *client* of the simmath shared
 *       library (dllimport)
 *   (3) we are building the simmath static library, or the client is
 *       being compiled with the expectation of linking with the
 *       simmath static library (nothing special needed)
 * In the CMake script for building this library, we define one of the symbols
 *     SIMMATH_BUILDING_{SHARED|STATIC}_LIBRARY
 * Client code normally has no special symbol defined, in which case we'll
 * assume it wants to use the shared library. However, if the client defines
 * the symbol SimTK_USE_STATIC_LIBRARIES we'll suppress the dllimport so
 * that the client code can be linked with static libraries. Note that
 * the client symbol is not library dependent, while the library symbols
 * affect only the simmath library, meaning that other libraries can
 * be clients of this one. However, we are assuming all-static or all-shared.
*/


#include <limits.h>
#include "SimTKcommon.h"
#include "SimTKmath.h"
#include "SimTKcommon/internal/BigMatrix.h"
#include "internal/common.h"

namespace SimTK {

//  default for recipricol of the condition number
static const double DefaultRecpCondition = 0.01;

class SimTK_SIMMATH_EXPORT Factor {
public:

  Factor() {}
  template <class ELT> Factor( Matrix_<ELT> m );
  template <class ELT> void solve( const Vector_<ELT>& b, Vector_<ELT>& x ) const;
  template <class ELT> void solve( const Matrix_<ELT>& b, Matrix_<ELT>& x ) const;
  
// TODO suppress implicit conversions ?
//   explicit Factor(class FactorRep* r) : rep(r) { }

}; // class Factor

class FactorLURepBase;

class SimTK_SIMMATH_EXPORT FactorLU: public Factor {
    public:

    ~FactorLU();

    FactorLU();
    FactorLU( const FactorLU& c );
    FactorLU& operator=(const FactorLU& rhs);

    template <class ELT> FactorLU( const Matrix_<ELT>& m );
    template <class ELT> void factor( const Matrix_<ELT>& m );
    template <class ELT> void solve( const Vector_<ELT>& b, Vector_<ELT>& x ) const;
    template <class ELT> void solve( const Matrix_<ELT>& b, Matrix_<ELT>& x ) const;

    template <class ELT> void getL( Matrix_<ELT>& l ) const;
    template <class ELT> void getU( Matrix_<ELT>& u ) const;
    template < class ELT > void inverse(  Matrix_<ELT>& m ) const;

    bool isSingular() const;
    int getSingularIndex() const;

     // only for symmetric/Hermitan, positive definite, tridaiagoal 
     // or symmetric/Hermitan indefinite => LDL
    template <class ELT> void getD( Matrix_<ELT>& m ) const; 

    protected:
    class FactorLURepBase *rep;

}; // class FactorLU


class FactorQTZRepBase;

class SimTK_SIMMATH_EXPORT FactorQTZ: public Factor {
    public:

    ~FactorQTZ();

    FactorQTZ();
    FactorQTZ( const FactorQTZ& c );
    FactorQTZ& operator=(const FactorQTZ& rhs);

    template <typename ELT> FactorQTZ( const Matrix_<ELT>& m);
    template <typename ELT> FactorQTZ( const Matrix_<ELT>& m, double rcond );
    template <typename ELT> FactorQTZ( const Matrix_<ELT>& m, float rcond );
    template <typename ELT> void factor( const Matrix_<ELT>& m);
    template <typename ELT> void factor( const Matrix_<ELT>& m, float rcond );
    template <typename ELT> void factor( const Matrix_<ELT>& m, double rcond );
    template <class ELT> void solve( const Vector_<ELT>& b, Vector_<ELT>& x ) const;
    template <class ELT> void solve( const Matrix_<ELT>& b, Matrix_<ELT>& x ) const;

    template < class ELT > void inverse(  Matrix_<ELT>& m ) const;

    int getRank() const;
//    void setRank(int rank);
    void setRecepricolConditionNumber( Real rcond ); 

    protected:
    class FactorQTZRepBase *rep;

}; // class FactorQTZ

class SimTK_SIMMATH_EXPORT Eigen {
    public:

    ~Eigen();

    Eigen();
    Eigen( const Eigen& c );
    Eigen& operator=(const Eigen& rhs);

    template <class ELT> Eigen( const Matrix_<ELT>& m );
    template <class ELT> void factor( const Matrix_<ELT>& m );
    template <class VAL, class VEC> void getAllEigenValuesAndVectors( Vector_<VAL>& values, Matrix_<VEC>& vectors);
    template <class T> void getAllEigenValues( Vector_<T>& values);

    template <class VAL, class VEC> void getFewEigenValuesAndVectors( Vector_<VAL>& values, Matrix_<VEC>& vectors, int ilow, int ihi);
    template <class T> void getFewEigenVectors( Matrix_<T>& vectors, int ilow, int ihi );
    template <class T> void getFewEigenValues( Vector_<T>& values, int ilow, int ihi );

    template <class VAL, class VEC> void getFewEigenValuesAndVectors( Vector_<VAL>& values, Matrix_<VEC>& vectors, typename CNT<VAL>::TReal rlow, typename CNT<VAL>::TReal rhi);
    template <class T> void getFewEigenVectors( Matrix_<T>& vectors, typename CNT<T>::TReal rlow, typename CNT<T>::TReal rhi );
    template <class T> void getFewEigenValues( Vector_<T>& values, typename CNT<T>::TReal rlow, typename CNT<T>::TReal rhi );

     
    protected:
    class EigenRepBase *rep;

}; // class Eigen

class SimTK_SIMMATH_EXPORT FactorSVD: public Factor {
    public:

    ~FactorSVD();

    FactorSVD();
    FactorSVD( const FactorSVD& c );
    FactorSVD& operator=(const FactorSVD& rhs);

    template < class ELT > FactorSVD( const Matrix_<ELT>& m );
    template < class ELT > FactorSVD( const Matrix_<ELT>& m, float rcond );
    template < class ELT > FactorSVD( const Matrix_<ELT>& m, double rcond );
    template < class ELT > void factor( const Matrix_<ELT>& m );
    template < class ELT > void factor( const Matrix_<ELT>& m, float rcond );
    template < class ELT > void factor( const Matrix_<ELT>& m, double rcond );

    template < class T > void getSingularValuesAndVectors( Vector_<typename CNT<T>::TReal>& values, 
                              Matrix_<T>& leftVectors,  Matrix_<T>& rightVectors );
    template < class T > void getSingularValues( Vector_<T>& values);

    int getRank();
    template < class ELT > void inverse(  Matrix_<ELT>& m );
    template <class ELT> void solve( const Vector_<ELT>& b, Vector_<ELT>& x );
    template <class ELT> void solve( const Matrix_<ELT>& b, Matrix_<ELT>& x );

    protected:
    class FactorSVDRepBase *rep;

}; // class FactorSVD

} // namespace SimTK 

#endif //SimTK_LINEAR_ALGEBRA_H_