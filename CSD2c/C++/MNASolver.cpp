#ifdef __linux__
  #if __has_include("/opt/intel/compilers_and_libraries_2020.0.166/linux/mkl/include/mkl.h")
    #include </opt/intel/compilers_and_libraries_2020.0.166/linux/mkl/include/mkl.h>
  #endif

#elif __APPLE__
  #if __has_include("/opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h")
    #include </opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h>
  #endif
#endif


#include <iosfwd>
#include <cmath>
#include "IComponent.h"
#include "MNASolver.h"

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


    void MNASolver::setSize(int size, double timestep, MNASystem & m)
    {

        nets = size;
        rNets = size-1;
        tStep = timestep;


         systemA = new double[rNets*rNets];
         systemA_lu = new float[rNets*rNets];
         pivot = new int[rNets*rNets];
         systemB = new double[rNets];
         systemX = new double[rNets];



         /*
					sysA = freeaml::Vector<double>(systemA, systemA+(rNets*rNets));
	 				A = freeaml::SparseMatrix<double>(rNets, rNets, sysA);
	 				x = freeaml::Vector<double>(systemX, systemX+rNets);
	 				b = freeaml::Vector<double>(systemB, systemB+rNets);


         for (int i = 0; i < rNets; i++ ) {
             systemB[i] = m.b[i+1].lu;
             systemX[i] = 0;
             for (int j = 0; j < rNets; j++ ) {
                 systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu; //klopt dit??
             }
         } */





    }



    void MNASolver::solve(std::vector<IComponent*> &components, MNASystem & m)
    {
        int iter;

        for(iter = 0; iter < maxIter; ++iter)
        {
          //std::cout << "oude iter "<< iter << '\n';
            // restore matrix state and add dynamic values
            updatePre(tStep, m);

            if(nets > 1) {

                luFactor(m);

                luForward(m);

                luSolve(m);

                if(newton(components, m)) break;
            }
        }


    }





        void MNASolver::solveMKL(std::vector<IComponent*> &components, MNASystem & m)
        {

                  int iter;

                  for(iter = 0; iter < 1; ++iter)
                  {


											updatePre(tStep, m);


											for (int i = 0; i < rNets; i++ ) {
													systemB[i] = m.b[i+1].lu;
													for (int j = 0; j < rNets; j++ ) {
															systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu;
													}
											}

												dgesv_(&rNets, &one, systemA, &rNets, pivot, systemB, &rNets, &info);
												m.b[0].lu = 0;
												for (size_t i = 1; i < nets; i++) {
													 m.b[i].lu = systemB[i-1];

											 }

                      if(newton(components, m)) break;

                }

      }


/*
      void solveEigen(std::vector<IComponent*> &components, MNASystem & m)
      {

          Eigen::VectorXd mX(rNets);


          Eigen::SparseMatrix<double> mA(rNets, rNets);
          Eigen::VectorXd mB(rNets);

          for (size_t i = 0; i < rNets; i++) {
            mX(i) = m.b[i+1].lu;
          }

          updatePre(tStep, m);

          for (int i = 0; i < rNets; i++ ) {
              mB(i) = m.b[i+1].lu;
              for (int j = 0; j < rNets; j++ ) {
                  mA.coeffRef(i, j) = m.A[j+1][i+1].lu; //klopt dit??
              }
          }

          solver.compute(mA);

          mX = solver.solve(mB);

          for (size_t i = 0; i < rNets; i++) {
            m.b[i+1].lu = mX(i);
          }
        }



      void solveAML(std::vector<IComponent*> &components, MNASystem & m)
      {

				for (int i = 0; i < rNets; i++ ) {
						x[i] = m.b[i+1].lu;
				}
				//std::vector<double> sysA(rNets);
				updatePre(tStep, m);

        //

        luFactor(m);

        freeaml::GeneralizedMinimumResidual<double> lss(1, 1e-5);
        //freeaml::GaussianElimination lss;

				for (int i = 0; i < rNets; i++ ) {
						b[i] = m.b[i+1].lu;
						for (int j = 0; j < rNets; j++ ) {
								systemA[(i*(rNets))+j] = m.A[i+1][j+1].lu;
						}
				}

				sysA = freeaml::Vector<double>(systemA, systemA+(rNets*rNets));
				A = freeaml::SparseMatrix<double>(rNets, rNets, sysA);
				//x = freeaml::Vector<double>(systemX, systemX+rNets);
				//b = freeaml::Vector<double>(systemB, systemB+rNets);


				lss.solve(A, x, b);

				for (int i = 0; i < rNets; i++ ) {
							 m.b[i+1].lu = x[i];

				}


				double residual = (A * x - b).l2_norm();


    }

 */


    // return true if we're done
    bool MNASolver::newton(std::vector<IComponent*> &components, MNASystem & m)
    {
        bool done = 1;
        for(int i = 0; i < components.size(); ++i)
        {
            done &= components[i]->newton(m);
        }
        return done;
    }


    void MNASolver::updatePre(double stepScale, MNASystem & m)
    {
        for(int i = 0; i < nets; ++i)
        {
            m.b[i].updatePre(stepScale);
            for(int j = 0; j < nets; ++j)
            {
                m.A[i][j].updatePre(stepScale);
            }
        }
    }

    void MNASolver::luFactor( MNASystem & m)
    {
        int p;
        for(p = 1; p < nets; ++p)
        {
            // FIND PIVOT
            {
                int pr = p;
                for(int r = p; r < nets; ++r)
                {
                    if(fabs(m.A[r][p].lu)
                            > fabs(m.A[pr][p].lu))
                    {
                        pr = r;
                    }
                }
                // swap if necessary
                if(pr != p)
                {
                    std::swap(m.A[p], m.A[pr]);
                    std::swap(m.b[p], m.b[pr]);
                }
            }
            if(0 == m.A[p][p].lu)
            {
                printf("Failed to find a pivot!!");
                printf("ERROR: Invalid circuit");
                throw;
                return;
            }

            // take reciprocal for D entry
            m.A[p][p].lu = 1 / m.A[p][p].lu;

            // perform reduction on rows below
            for(int r = p+1; r < nets; ++r)
            {
                if(m.A[r][p].lu == 0) continue;

                m.A[r][p].lu *= m.A[p][p].lu;
                for(int c = p+1; c < nets; ++c)
                {
                    if(m.A[p][c].lu == 0) continue;

                    m.A[r][c].lu -=
                        m.A[p][c].lu * m.A[r][p].lu;
                }

            }
        }
    }

    // this does forward substitution for the solution vector
    int MNASolver::luForward( MNASystem & m)
    {
        int p;
        for(p = 1; p < nets; ++p)
        {
            // perform reduction on rows below
            for(int r = p+1; r < nets; ++r)
            {
                if(m.A[r][p].lu == 0) continue;
                if(m.b[p].lu == 0) continue;
                // A.lu bevat 1, 0 of -1. 0 is al uitgesloten. systeem b gaat + of - zichzelf maar wordt daardoor 0
                m.b[r].lu -= m.b[p].lu * m.A[r][p].lu;
            }
        }
        return p;
    }

    // solves nodes backwards from limit-1
    int MNASolver::luSolve( MNASystem & m)
    {
        for(int r = nets; --r;)
        {
            //printf("solve node %d\n", r);
            for(int s = r+1; s < nets; ++s)
            {
                m.b[r].lu -= m.b[s].lu * m.A[r][s].lu;
            }

            m.b[r].lu *= m.A[r][r].lu;
        }
        return 1;
    }
