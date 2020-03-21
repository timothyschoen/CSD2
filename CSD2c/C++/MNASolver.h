#pragma once
#ifdef __linux__
  #if __has_include("/opt/intel/compilers_and_libraries_2020.0.166/linux/mkl/include/mkl.h")
    #include </opt/intel/compilers_and_libraries_2020.0.166/linux/mkl/include/mkl.h>
  #endif

#elif __APPLE__
  #if __has_include("/opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h")
    #include </opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h>
  #endif
#endif


#include <iostream>
#include <cmath>
#include "IComponent.h"

/*
#include "./IterativeLinearSolvers.h"
#include "SparseLU"

// prerequisites
#include "./freeaml/Matrix.h"
#include "./freeaml/Vector.h"
#include "./freeaml/SparseMatrix.h"

// Solvers
#include "./freeaml/IncompleteCholeskyConjugateGradient.h"
#include "./freeaml/SuccessiveOverRelaxation.h"
#include "./freeaml/GaussianElimination.h"
#include "./freeaml/BidiagonalFactorization.h"
#include "./freeaml/GeneralizedMinimumResidual.h"
#include "./freeaml/HessenbergFactorization.h"
#include "./freeaml/MinimumResidual.h"
#include "./freeaml/PLUFactorization.h"
#include "./freeaml/SteepestDescent.h"
#include "./freeaml/WeightedJacobi.h"

#include "./ilupp/iluplusplus.h"

extern "C" {    // another way
      #include "csparse.h"
    };
 */


struct MNASolver
{
    int nets;
    int rNets;
    int maxIter = 20;
    double tStep;
    double *systemA;
    float *systemA_lu;
    int *pivot;
    double *systemB;
    double *systemX;

    int info;
    int one = 1;

    int iter;
    void *pt[64];
    MKL_INT maxfct, mnum, error, msglvl;
    int phase = 13;
    int nz = 0;
    MKL_INT i, j;
    double ddum;          /* Double dummy */
    MKL_INT idum;         /* Integer dummy. */
    MKL_INT nrhs = 1;     /* Number of right hand sides. */
    MKL_INT mtype = -99;       /* Real unsymmetric matrix */
    MKL_INT iparm[64];


    /*

		freeaml::Vector<double> sysA;

		freeaml::SparseMatrix<double> A;

		freeaml::Vector<double> x;

		freeaml::Vector<double> b;
*/

    //Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    //Eigen::IncompleteLUT<Eigen::SparseMatrix<double> > solver;
    //Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IncompleteLUT<double> >  solver;
    //Eigen::ConjugateGradient<Eigen::MatrixXd, Eigen::Lower|Eigen::Upper> cg;

		//freeaml::SuccessiveOverRelaxation<double> lss(20, 1e-2, 0.4);
		//freeaml::IncompleteCholeskyConjugateGradient lss(5, 1e-9);
		//freeaml::BiconjugateGradientStabilized<double> lss(50, 1e-12);


    void setSize(int size, MNASystem & m);

    void solve(std::vector<IComponent*> &components, MNASystem & m);

    void solveMKL(std::vector<IComponent*> &components, MNASystem & m);

    void solveEigen(std::vector<IComponent*> &components, MNASystem & m);

    void solveAML(std::vector<IComponent*> &components, MNASystem & m);



protected:

    // return true if we're done
    bool newton(std::vector<IComponent*> &components, MNASystem & m);


    void updatePre(MNASystem & m);

    void luFactor( MNASystem & m);

    // this does forward substitution for the solution vector
    int luForward( MNASystem & m);

    // solves nodes backwards from limit-1
    int luSolve( MNASystem & m);


};
