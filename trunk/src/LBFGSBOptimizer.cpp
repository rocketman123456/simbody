

/* Portions copyright (c) 2006 Stanford University and Jack Middleton.
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
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "f2c.h"
#include "LBFGSBOptimizer.h"

using std::cout;
using std::endl;
int setulb_(integer *n, integer *m, doublereal *x, doublereal *l,
      doublereal *u, integer *nbd, doublereal *f, doublereal *g,
      doublereal *factr, doublereal *pgtol, doublereal *wa, integer *iwa,
      char *task, integer *iprint, char *csave, logical *lsave,
      integer *isave, doublereal *dsave, ftnlen task_len, ftnlen csave_len);

const int NUMBER_OF_CORRECTIONS = 5;
namespace SimTK {


     LBFGSBOptimizer::LBFGSBOptimizer( OptimizationProblem& p ){
        printf(" LBFSBOptimizer constructor \n");         
          int m,i;
          char buf[1024];

         pop = &p;


         if( p.dimension < 1 ) {
             char *where = "Optimizer Initialization";
             char *szName= "dimension";
             SimTK_THROW5(SimTK::Exception::ValueOutOfRange, szName, 1,  p.dimension, INT_MAX, where); }

          /* assume all paramters have both upper and lower bounds */
          nbd = (int *)malloc(p.numBounds*sizeof(int));
          for(i=0;i<p.numBounds;i++) {
               nbd[i] = 2;
          }


          numCorrections = m = NUMBER_OF_CORRECTIONS;
          dimension = p.dimension;
          gradient = new SimTK::Vector(dimension);

     } 
     LBFGSBOptimizer::LBFGSBOptimizer(){ 
        printf(" LBFSBOptimizer default constructor \n"); 
     }


     double LBFGSBOptimizer::optimize( double *results ) {
        printf("call C interface for LBFGSB optimize \n");
/* TODO C interface 
*/
         return(0.0);

     }
     double LBFGSBOptimizer::optimize(  SimTK::Vector &results ) {

         int i;
         int run_optimizer = 1;
         int iflag[1] = {0};
         char task[61];
         double f;
         double factr = 1.0e7;   // 
         double pgtol = 1.0e-5;
         int iprint = 1;
         int *iwa;
         int n = dimension;
         char csave[61];
         logical lsave[4];
         int isave[44];
         double dsave[29];
         double *wa;
         SimTK::Vector &grad=  *gradient;

        printf("call LBFGSB optimize \n");

         iwa = (int *)malloc(3*n*sizeof(int));
         wa = (double *)malloc( ((2*n + 4)*n + 12*n*n + 12*pop->numBounds)*sizeof(double));

                strcpy( task, "START" );
         while( run_optimizer ) { 
            setulb_(&dimension, &numCorrections, &results[0], pop->lower_bounds,
                    pop->upper_bounds, nbd, &f, &grad[0],
                    &factr, &pgtol, wa, iwa,
                    task, &iprint, csave, lsave, isave, dsave, 60, 60);
             if( strncmp( task, "FG", 2) == 0 ) {
                f = pop->objectiveAndGradient( dimension, true, results,  grad, pop->user_data );

             } else if( strncmp( task, "NEW_X", 5) != 0 ){
               printf("LBFGSB terminated \n");
                f = pop->objectiveAndGradient( dimension, true, results,  grad, pop->user_data );
               run_optimizer = 0;
            }
         }

         return(f);
      }

      unsigned int LBFGSBOptimizer::optParamStringToValue( char *parameter )  {

         unsigned int param;
         char buf[1024];

         if( 0 == strncmp( "FUNCION_EVALUATIONS", parameter, 1) ) {
           param = MAX_FUNCTION_EVALUATIONS;
         } else if( 0 == strncmp( "STEP_LENGTH", parameter, 1)) {
           param = DEFAULT_STEP_LENGTH;
         } else if( 0 == strncmp( "TOLERANCE", parameter, 1)) {
           param = TRACE;
         } else if( 0 == strncmp( "GRADIENT", parameter, 1)) {
           param = GRADIENT_CONVERGENCE_TOLERANCE;
         } else if( 0 == strncmp( "ACCURACY", parameter, 1)) {
           param = LINE_SEARCH_ACCURACY;
         } else {
             sprintf(buf," Parameter=%s",parameter);
             SimTK_THROW1(SimTK::Exception::UnrecognizedParameter, SimTK::String(buf) ); 
         }

         return( param );

      }

     void LBFGSBOptimizer::setOptimizerParameters(unsigned int parameter, double *values ) { 
          int i;
          char buf[1024];

          printf("call LBFGSB setOptimizerParameters \n");
          switch( parameter) {
             case MAX_FUNCTION_EVALUATIONS:
                   MaxNumFuncEvals = (unsigned int)values[0];
                   break;
             case DEFAULT_STEP_LENGTH:
                   DefaultStepLength = (unsigned int)values[0];
                   break;
             case LINE_SEARCH_ACCURACY:
                   LineSearchAccuracy = values[0];
                   break;
             case  GRADIENT_CONVERGENCE_TOLERANCE:
                   GradientConvergenceTolerance = values[0];
                   break;
             default:
                   sprintf(buf," Parameter=%d",parameter);
                   SimTK_THROW1(SimTK::Exception::UnrecognizedParameter, SimTK::String(buf) ); 
                   break;
          }

        return; 

      }
     void LBFGSBOptimizer::getOptimizerParameters(unsigned int parameter, double *values ) {
          int i;
          char buf[1024];

        printf("call LBFGSB getOptimizerParameters \n");

            switch( parameter) {
               case MAX_FUNCTION_EVALUATIONS:
                     values[0] = (double )MaxNumFuncEvals;
                     break;
               case DEFAULT_STEP_LENGTH:
                     values[0] = DefaultStepLength;
                     break;
               case LINE_SEARCH_ACCURACY:
                     values[0] = LineSearchAccuracy;
                     break;
               case  GRADIENT_CONVERGENCE_TOLERANCE:
                      values[0] = GradientConvergenceTolerance;
                      break;
               default:
                      sprintf(buf," Parameter=%d",parameter);
                      SimTK_THROW1(SimTK::Exception::UnrecognizedParameter, SimTK::String(buf) ); 
                      break;
            }
  
          return; 
   }
} // namespace SimTK