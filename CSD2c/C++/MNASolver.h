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



struct MNASolver
{
    int nets; // total number of nets
    int rNets; // nets without ground


    double *systemA; // Maps to m.A[x][y].lu
    double *systemB; // Maps to m.b[x].lu

    int iter;
    int maxIter = 1;

    int info;
    int *pivot;
    int one = 1;



    void setSize(int size, MNASystem & m);

    void setIterations(int iterations);


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
