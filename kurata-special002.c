/*
  Self-Organizing Map:  A demo using C language and gnuplot !!!
  
  2005. 4.15 by A.Date
  Last modified 31 Jul 2020
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> /* for usleep */

#define N_XUNITS 20
#define N_YUNITS  1
#define DIM_INPUT 3 /* dimension of input */

#define Z_RANGE 1.0

// #define SLEEP 1000
#define SLEEP 100

double X_RANGE = 0.01;
double Y_RANGE = 0.01;

void som2d();
void init_reference();
void write_data();

double ALPHA=0.01;
double SIGMA=0.8;
int N_LEARNING = 10000;
int N_SUB_LEARNING = 1000;

double S[DIM_INPUT];    /* input */

double RV[N_XUNITS][N_YUNITS][DIM_INPUT];
int T[N_XUNITS][N_YUNITS]; 

FILE *gp;


typedef struct _Position Position;
struct _Position {
	int  x;
	int  y;
};


int main (int argc, char *argv[] )
{
  int i,j;
  int r1, r2;
  double x,y;
  long seed = 1234567;
  
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
  fprintf(gp, "set xlabel \"x\"\n");
  fprintf(gp, "set ylabel \"y\"\n");
  fprintf(gp, "set zlabel \"z\"\n");
  
  fprintf(gp, "set xrange[-1:1]\n");
  fprintf(gp, "set yrange[-1:1]\n");
  fprintf(gp, "set zrange[0:1]\n");
  
  fprintf(gp, "set nokey\n");
  //	fprintf(gp, "set view 0,0\n");
  fflush(gp);
  
  while(1){
    
    init_reference();
    for(i=0;i<N_LEARNING;i++){
      fprintf(gp, "splot '-'\n");
      fflush(gp); 
      
      for(j=0;j<N_SUB_LEARNING;j++){
	som2d();
      }
      X_RANGE = X_RANGE + 0.001;
      
      write_data();
      fprintf(gp,"e\n");
      fprintf(gp, "set title \"t=%d\"\n",i*j+j);
      fflush(gp); 
      
      
      r1 = i % 360;
      r2 = j % 20;
      fprintf(gp, "set view %d,%d\n",r2+50, r1);
      fflush(gp); 
      
      usleep(SLEEP*100);
    }
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
  
  /*
    1.  Choose an input x randomly.  
    2.  Find the winnder, and apply neighborhood learning rule.
  */ 
  
  int i,j,k;
  double tmp;
  double min;
  double denom;
  double hci;
  double dist;
  double sum = 0.0;
  
  Position argmin; 
  
  /* 	Choose an input x randomly.  */
  
  
  S[0] = X_RANGE;
  S[1] = X_RANGE;
  while ( sqrt(S[0]*S[0] + S[1]*S[1]) > X_RANGE ){
    S[0] = drand48()*X_RANGE;
    S[1] = drand48()*X_RANGE;
  }
  S[0] = S[0] - 0.5*X_RANGE;
  S[1] = S[1] - 0.5*X_RANGE;
  S[2] = drand48()*Z_RANGE;
  
  /*
    for (k=0; k<DIM_INPUT; k++){
    S[k] = drand48();
    }
  */
  
  /* find closest ref. vector */
  
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
  
  /* learning,  c = argmin */
  
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

void
write_data(){
  
  int i,j;
  
  /* write data for gnuplot */
  
  for (i=0; i<N_XUNITS; i++){
    for (j=0; j<N_YUNITS; j++){
      fprintf(gp, "%.5lf %.5lf %.5lf\n", RV[i][j][0], RV[i][j][1], RV[i][j][2]); 
    }
    fprintf(gp, "\n"); 
  }		
  
  for (j=0; j<N_YUNITS; j++){
    for (i=0; i<N_XUNITS; i++){
      fprintf(gp, "%.5lf %.5lf %.5lf\n", RV[i][j][0], RV[i][j][1], RV[i][j][2]); 
    }
    fprintf(gp, "\n"); 
  }
  
}
