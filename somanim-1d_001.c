/*
  Self-Organizing Map (1D)
  
  2006.5.14  A.Date
  Last modified 31 Jul 2029
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> /* for usleep */

#define N_XUNITS 20
#define DIM_INPUT 1 /* dimension of input signal space */

double ALPHA=0.2;  /* coefficients for neighborhood learning */
double SIGMA=0.8;

int SLEEP=100000;  /* set small number if it is too slow. */
int N_LEARNING = 500;
int N_SUB_LEARNING = 10;

double X_RANGE=1.0;
double Y_RANGE=1.0;

double S[DIM_INPUT];    /* input signal */
double RV[N_XUNITS][DIM_INPUT]; /* reference vectors */

FILE *gp;

void som1d();
void init_reference();
void write_data_anim_d1();

/*
  gnuplot can read data from standard input (without data file).
  after "plot '-'", send data. gnuplot read data until 'e' appear.
*/

int main (int argc, char *argv[] )
{
  int i,j,k;
  double x,y;
  long seed = 1234567; /* set your favorite number */
  
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
  //  fprintf(gp, "set term aqua\n"); // for mac
  fprintf(gp, "set parametric\n");
  fprintf(gp, "set style data linespoints\n");
  fprintf(gp, "set xlabel \"neural fields\"\n");
  fprintf(gp, "set xrange[-0.2:1.2]\n");
  fprintf(gp, "set yrange[0:1]\n");
  fprintf(gp, "set border 5\n"); 
  fprintf(gp, "set x2tics 0,0.2,1\n");
  fprintf(gp, "set noxtics\n");
  fprintf(gp, "set noytics\n");
  fprintf(gp, "set nokey\n");
  fflush(gp);
  
  init_reference();
  for(i=0;i<N_LEARNING;i++){
    fprintf(gp, "set title 't = %d/%d'\n", i*N_SUB_LEARNING, N_LEARNING*N_SUB_LEARNING);
    fprintf(gp, "plot '-'\n");
    
    for(j=0;j<N_SUB_LEARNING;j++){
      som1d();
    }
    
    write_data_anim_d1();
    fprintf(gp,"e\n");
    fflush(gp); 
    usleep(SLEEP);
  }
  
  fclose(gp);
}


/* set initial connection */
void
init_reference(){ 
  
  int i,k; 
  for (i=0; i < N_XUNITS; i++){
    for (k=0; k<DIM_INPUT; k++){
      //			RV[i][k] = drand48();
      RV[i][k] = (double)i/(double)N_XUNITS  + (25.0/(double)N_XUNITS)*(drand48()-0.5);
    }       
  }       
  
}


void
som1d()
{
  int i,k;
  int argmin, d; 
  double tmp, min, denom, hci;
  double sum = 0.0;
  
  /* 	Choose an input x randomly. Change here if you want strange results */
  S[0] = drand48()*X_RANGE;
  
  /*
    if ( drand48() > 0.5 ){
    S[0] = 0.75 + 0.2*(drand48()-0.5);
    }
    else{
    S[0] = 0.25 + 0.2*(drand48()-0.5);
    }
  */
  //	printf("%.5lf\n",S[0]);
  
  /* find the winner unit which has reference vector closest to the input */
  for (k=0; k<DIM_INPUT; k++){
    tmp = RV[0][k] - S[k];
    sum += tmp*tmp;
  }
  argmin = 0; 
  min = sum; 
  
  for (i=0; i<N_XUNITS; i++){
    sum = 0.0;
    for (k=0; k<DIM_INPUT; k++){
      tmp = RV[i][k] - S[k];
      sum += tmp*tmp;
    }
    if ( sum < min ){
      min = sum;
      argmin = i; 
    } 
  } 
  
  /* neighborhood learning,  c = argmin */
  for (i=0; i<N_XUNITS; i++){
    d = (i-argmin)*(i-argmin);
    denom = 2.0*SIGMA*SIGMA;
    hci = ALPHA*exp(-(double)d/denom);
    
    for (k=0; k<DIM_INPUT; k++){ 
      RV[i][k] =  RV[i][k] + hci* (S[k] -RV[i][k]);
    } 
  } 
  
}


/* write data for gnuplot */
void
write_data_anim_d1(){
  
  int i;
  for (i=0; i<N_XUNITS; i++){
    fprintf(gp, "%.5lf %.5lf\n", RV[i][0], 0.99); 
    fprintf(gp, "%.5lf %.5lf\n", (double)i/(double)N_XUNITS+0.01, 0.01); 
    fprintf(gp, "\n"); 
  }		

}
