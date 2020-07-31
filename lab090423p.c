/* 
   lab090423p.c
   
   Simple Neuron
   
   2009.4.23 A.Date
   Last modified at 31 Jul 2020
   
   %  gcc lab090423p.c -lm
   
*/

#include <stdio.h>
#include <stdlib.h> /* drand48 */
#include <math.h> /* log */
#include <unistd.h> /* for usleep */

#define MAX_SAMPLES 1000
#define N_DIM 3

long int RAND_SEED = 20090423;
int SLEEP = 50000;  /* adjustment of display speed */
int N_DEMO = 30;

int n_data;
int n_data1 = 20;
int n_data2 = 20;
int n_trials = 100;  /* number of learnings */

double eta = 0.1;    /* learning coefficient */
double lambda=1.0;   /* a parameter for sigmoid function */
double SIGMA = 1.0;

char data_file1[] = "apples100.dat";   /* file name of data. need to change for project C,D */
char data_file2[] = "oranges100.dat";

double nrand();

double sigmoid(double x){
  return 1.0/(1.0 + exp(-lambda*x));
}


int main (int argc, char *argv[] )
{
  FILE *gp, *fp; 
  
  int a,i,k,l,t, n;
  long int seed = RAND_SEED;
  
  int n_data;
  int teacher; 
  
  double mu1x,mu1y,mu2x,mu2y;
  double y1, y2; 
  double dw[N_DIM];
  double w[N_DIM];
  double data[MAX_SAMPLES][N_DIM];
  double u,z,e,e2; 
  double x_start = -5.0, x_end=5.0;
  double y_start = -5.0, y_end=5.0;
  
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
  n_data = n_data1 + n_data2;
  
  /* command for gnuplot : do not mind too much */
  gp = popen("gnuplot","w");
  fprintf(gp, "set xlabel \"x1\"\n");
  fprintf(gp, "set ylabel \"x2\"\n");
  fprintf(gp, "set xrange[%.2lf:%.2lf]\n",x_start,x_end);
  fprintf(gp, "set yrange[%.2lf:%.2lf]\n",y_start,y_end);
  fprintf(gp, "set size square\n");
  //      fprintf(gp, "set terminal postscript\n");
  
  for (k=0; k<N_DEMO; k++) {
    
    /* generate data */
    mu1x = nrand();
    mu1y = nrand();
    mu2x = nrand();
    mu2y = nrand();
    
    fp = fopen(data_file1, "w");
    if (fp == NULL){
      fprintf(stderr, "Can't write data file: %s\n",data_file1);
      exit(-1);
    }
    for (i=0; i<n_data1; i++) {
      data[i][1] = mu1x  + SIGMA*nrand();
      data[i][2] = mu1y  + SIGMA*nrand();
      fprintf( fp, "%.8lf \t %.8lf\n",data[i][1],data[i][2]);
    }
    fclose( fp );
    
    fp = fopen(data_file2, "w");
    if (fp == NULL){
      fprintf(stderr, "Can't write data file: %s\n",data_file2);
      exit(-1);
    }
    
    for (i=n_data1; i<n_data; i++) {
      data[i][1] = mu2x  + SIGMA*nrand();
      data[i][2] = mu2y  + SIGMA*nrand();
      fprintf(fp, "%.8lf \t %.8lf\n",data[i][1],data[i][2]);
    }
    fclose( fp );
    
    for (i=0; i<n_data; i++) {
      data[i][0] = 1.0;
    }
    
    
    /* set random values to the weight connections */
    for (i=0; i<N_DIM; i++) {
      w[i] = drand48() -0.5;
    }
    
    /* loop over a training set */
    for (t=0; t<n_trials; t++) { 
      dw[0] = 0.0;
      dw[1] = 0.0;
      dw[2] = 0.0;
      e2 = 0.0;
      for ( i = 0; i<n_data; i++) {
	u = w[0] + w[1]*data[i][1] +  w[2]*data[i][2];
	
	if ( i < n_data1 ){
	  teacher = 1;
	}
	else{
	  teacher = 0;
	}
	
	z = sigmoid(u);
	e = (double)teacher - z;
	e2 = e2 + e*e;
	
	/* comment out if you want to see the dynamics of unit activity.*/
	// printf("t:%d\t pattern:%d\t z:%.5lf\t true:%d\t error:%.5lf\n", t, i, z, teacher, e);
	
	dw[0] = dw[0] + 2.0*eta*e*lambda*z*(1.0-z);
	dw[1] = dw[1] + 2.0*eta*e*lambda*z*(1.0-z)*data[i][1];
	dw[2] = dw[2] + 2.0*eta*e*lambda*z*(1.0-z)*data[i][2];
      }
      
      /* comment out if you want to see something */
      // printf("t:%d \t e2:%.5lf\n", t, e2);
      
      /* update weights */	
      w[0] = w[0] + dw[0];
      w[1] = w[1] + dw[1];
      w[2] = w[2] + dw[2]; 
      
      /* comment out if you want to see dynamics of weights */
      //  printf("%d: %.5lf\t%.5lf\t%.5lf\n",t, dw[0],dw[1],dw[2]);
      
      /* plot at u=0 (hyper plane) */
      fprintf(gp, "set title 't =%d'\n", t);
      fprintf(gp, "plot '%s' with points pt 1, '%s' with points pt 3, %.3lf*x + %.3lf with lines\n", data_file1, data_file2, -w[1]/w[2], -w[0]/w[2]);
      fflush(gp); 
      usleep(SLEEP);
    } 
  }
}

/* nrand:  generate a data from standard normal distribution  */
double nrand() 
{
  static int sw=0;
  static double r1,r2,s; 
  
  if (sw==0){
    sw=1;
    do {
      r1=2.0*drand48()-1.0;
      r2=2.0*drand48()-1.0;
      s=r1*r1+r2*r2;
    } while (s>1.0 || s==0.0);
    s=sqrt(-2.0*log(s)/s);
    return(r1*s);
  }
  else {
    sw=0;
    return(r2*s);
  }
}
