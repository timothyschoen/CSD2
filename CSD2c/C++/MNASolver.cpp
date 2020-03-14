
//#include <Accelerate/Accelerate.h>
#include </opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h>
#include <iostream>
#include "./IterativeLinearSolvers.h"







struct MNASolver
{

    int nets;
    int rNets;
    int maxIter = 20;
    double tStep;
    float *systemA;
    float *systemA_lu;
    int *pivot;
    float *systemB;
    float *systemX;
    float *r;
    float *c;
    float *work;
    int *iwork;
    float rcond;
    float ferr;
    float berr;
    int info;
    char e = 'E';
    char n = 'N';
    char set;
    int zero = 0;
    int one = 1;
    float room;
    Eigen::BiCGSTAB<Eigen::MatrixXd> solver;
    Eigen::ConjugateGradient<Eigen::MatrixXd, Eigen::Lower|Eigen::Upper> cg;
    /*
      MNASolver(int size) : nets(size-1)
      {


      } */


public:

    void setSize(int size, double timestep, MNASystem & m)
    {

        nets = size;
        rNets = size-1;
        tStep = timestep;

         systemA = new float[rNets*rNets];
         systemA_lu = new float[rNets*rNets];
         pivot = new int[rNets*rNets];
         systemB = new float[rNets];
         systemX = new float[rNets];
         r = new float[rNets];
         c = new float[rNets];
         work = new float[rNets*rNets];
         iwork = new int[rNets*rNets];


         for (int i = 0; i < rNets; i++ ) {
             systemB[i] = (float)m.b[i+1].lu;
             for (int j = 0; j < rNets; j++ ) {
                 systemA[(i*(rNets))+j] = (float)m.A[j+1][i+1].lu; //klopt dit??
             }
         }





    }


    void solve(MNASystem & m)
    {

        updatePre(tStep, m);


        std::vector<double> systemA((rNets)*(rNets));
        std::vector<double> systemB(rNets);

        for (int i = 0; i < rNets; i++ ) {
            systemB[i] = m.b[i+1].lu;
            for (int j = 0; j < rNets; j++ ) {
                systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu; //klopt dit??
            }
        }



    }


    void solve2(MNASystem & m)
    {
        updatePre(tStep, m);


        std::vector<double> systemA((rNets)*(rNets));
        std::vector<double> systemB(rNets);

        for (int i = 0; i < rNets; i++ ) {
            systemB[i] = m.b[i+1].lu;
            for (int j = 0; j < rNets; j++ ) {
                systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu; //klopt dit??
            }
        }




    }

    void solve3(std::vector<IComponent*> &components, MNASystem & m)
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




    void solve4(MNASystem & m)
        {
          updatePre(tStep, m);

          Eigen::VectorXd x(nets);
          //Eigen::Map<Eigen::MatrixXf> mA(systemA, rNets, rNets);

          //Eigen::Map<Eigen::VectorXf> mB(systemB, rNets);

          Eigen::MatrixXd mA(nets, nets);
          Eigen::VectorXd mB(nets);

          for (size_t i = 1; i < nets; i++) {
            mB(i-1) = m.b[i].lu;
            x(i) = 0;
            for (size_t j = 1; j < nets; j++) {
              mA(i-1, j-1) = m.A[j][i].lu;
            }
          }

          cg.compute(mA);
          x = cg.solve(mB);
          //solver.compute(mA);
          //x = solver.solve(mB);

          for (size_t i = 0; i < nets; i++) {
            std::cout << x(i) << '\n';
            m.b[i+1].lu = x(i);
          }

        }



        void solve5(std::vector<IComponent*> &components, MNASystem & m)
        {

                  int iter;

                  for(iter = 0; iter < maxIter; ++iter)
                  {

                    updatePre(tStep, m);



                      //sgesv_(&rNets, &one, systemA, &rNets, pivot, systemB, &rNets, &info);




                     //ssysv_rk_("UPPER", &rNets, &one, systemA, &rNets, &room, pivot, systemB, &rNets, work, iwork, &info);

                       //la_solve();

                       m.b[0].lu = 0;
                       for (size_t i = 1; i < nets; i++) {
                          m.b[i].lu = systemB[i-1];

                      }
                      //std::cout << "nieuwe iter "<< iter << '\n';

                      if(newton(components, m)) break;

                      e = 'E';
                }

                  //e = 'F';
      }


      void solve6(std::vector<IComponent*> &components, MNASystem & m)
      {

    }



protected:

    // return true if we're done
    bool newton(std::vector<IComponent*> &components, MNASystem & m)
    {
        bool done = 1;
        for(int i = 0; i < components.size(); ++i)
        {
            done &= components[i]->newton(m);
        }
        return done;
    }


    void updatePre(double stepScale, MNASystem & m)
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

    void luFactor( MNASystem & m)
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
    int luForward( MNASystem & m)
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
    int luSolve( MNASystem & m)
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



};
