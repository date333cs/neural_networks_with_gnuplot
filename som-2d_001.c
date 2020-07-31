/*
  Self-Organizing Map (2D):
  2006.5.14  A.Date
  Last modified at 31 Jul 2020
  
  % gcc som-2d_001.c -lm 
  
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> /* for usleep */

#define N_XUNITS 10
#define N_YUNITS 10
#define DIM_INPUT 2 /* #dimensions of input signal*/

double X_RANGE=1.0;
double Y_RANGE=1.0;
int SLEEP=100000;
double ALPHA=0.2;
double SIGMA=0.8;
int N_LEARNING = 200;
int N_SUB_LEARNING = 100;

double S[2];    /* input */
double RV[N_XUNITS][N_YUNITS][DIM_INPUT];
int T[N_XUNITS][N_YUNITS]; 
FILE *gp;

void som2d();
void init_reference();
void write_data();

typedef struct _Position Position;
struct _Position {
  int  x;
  int  y;
};

int main (int argc, char *argv[] )
{
  int i,j;
  double x,y;
  long seed = 1234567;   /* set your favorite number */
  
  for (i=1; i<argc; i++) {
    switch (*(argv[i]+1)) {
    case 'r':
      seed = atoi(argv[++i]);
      break;
    default:
      fprintf(stderr, "Usage : %s\n",argv[0]);
      fprintf(stderr, "\t-r : random-seed(%ld)\n",seed) ;
      exit(0);
      break;
    }
  }
  
  srand48(seed);
  
  gp = popen("gnuplot","w");
  fprintf(gp, "set parametric\n");
  fprintf(gp, "set style data linespoints\n");
  fprintf(gp, "set xlabel \"x_1\"\n");
  fprintf(gp, "set ylabel \"x_2\"\n");
  fprintf(gp, "set xrange[0:1]\n");
  fprintf(gp, "set yrange[0:1]\n");
  fprintf(gp, "set nokey\n");
  fflush(gp);
  
  init_reference();
  for(i=0;i<N_LEARNING;i++){
    fprintf(gp, "set title 't = %d/%d'\n", i*N_SUB_LEARNING, N_LEARNING*N_SUB_LEARNING);
    fprintf(gp, "plot '-'\n");
    fflush(gp); 
    
    for(j=0;j<N_SUB_LEARNING;j++){
      som2d();
    }
    
    write_data();
    fprintf(gp,"e\n");
    
    fflush(gp); 
    usleep(SLEEP);
  }
  
  fclose(gp);
}


void
init_reference(){ 
  
  int i,j,k; 
  for (i=0; i < N_XUNITS; i++){
    for (j=0; j < N_YUNITS; j++){
      for (k=0; k<DIM_INPUT; k++){
        RV[i][j][k] = drand48();
      }
    }       
  }       
}


void
som2d()
{
  int i,j,k, dist;
  double tmp;
  double min;
  double denom;
  double hci;
  double sum = 0.0;
  
  Position argmin; 
  
  /* 	Choose an input x randomly.  
        Change here if you want something different. */	
  S[0] = drand48()*X_RANGE;
  S[1] = drand48()*Y_RANGE;
    
  /* find the closest ref. vector */
  for (k=0; k<DIM_INPUT; k++){
    tmp = RV[0][0][k] - S[k];
    sum += tmp*tmp;
  }
  argmin.x = 0; 
  argmin.y = 0; 
  min = sum; 
  
  for (i=0; i<N_XUNITS; i++){
    for (j=0; j<N_YUNITS; j++){
      sum = 0.0;
      for (k=0; k<DIM_INPUT; k++){
        tmp = RV[i][j][k] - S[k];
	      sum += tmp*tmp;
      }
      if ( sum < min ){
	      min = sum;
	      argmin.x = i; 
	      argmin.y = j; 
      } 
    } 
  }
  
  /* neighborhood learning rule,  c = argmin */
  for (i=0; i<N_XUNITS; i++){
    for (j=0; j<N_YUNITS; j++){
      dist = (i-argmin.x)*(i-argmin.x) +(j-argmin.y)*(j-argmin.y);
      denom = 2.0*SIGMA*SIGMA;
      hci = ALPHA*exp(-(double)dist/denom);
      
      for (k=0; k<DIM_INPUT; k++){ 
        RV[i][j][k] =  RV[i][j][k] + hci* (S[k] -RV[i][j][k]);
      } 
    }
  } 
  
}


/* write data for gnuplot. */
void
write_data(){
  
  int i,j;
  
  for (i=0; i<N_XUNITS; i++){
    for (j=0; j<N_YUNITS; j++){
      fprintf(gp, "%.5lf %.5lf\n", RV[i][j][0], RV[i][j][1]); 
    }
    fprintf(gp, "\n"); 
  }		
  
  for (j=0; j<N_YUNITS; j++){
    for (i=0; i<N_XUNITS; i++){
      fprintf(gp, "%.5lf %.5lf\n", RV[i][j][0], RV[i][j][1]); 
    }
    fprintf(gp, "\n"); 
  }
  
}
