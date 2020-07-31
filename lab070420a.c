/* 
   lab070420a.c

   Simple Neuron

   2007.4.19 A.Date
   Last modified at 31 Jul 2020

  %  gcc lab070420a.c -lm
 
 */

#include <stdio.h>
#include <stdlib.h> /* drand48 */
#include <math.h> /* log */
#include <unistd.h> /* for usleep */

#define MAX_SAMPLES 1000
#define N_DIM 3

#define RAND_SEED  123456789
#define SLEEP 50000
int n_trials = 100;
double eta = 0.1; 
double lambda=1.0;
char data_file1[] = "apples.dat";
char data_file2[] = "oranges.dat";

int n_data, n_data1, n_data2;

double sigmoid(double x){
	return 1.0/(1.0 + exp(-lambda*x));
}

int main (int argc, char *argv[] )
{
  FILE *gp, *fp; 
  
  int a,i,j,l,t, n;
  long int seed = RAND_SEED;
  
  int n_data;
  int teacher; 
  
  double y1, y2; 
  double dw[N_DIM];
  double w[N_DIM];
  double data[MAX_SAMPLES][N_DIM];
  double u,z,e,e2; 
  double x_start = -1.0, x_end=5.0;
  double y_start = -1.0, y_end=5.0;
  
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
  
  /* load data */
  fp = fopen(data_file1, "r");
  if (fp == NULL){
    fprintf(stderr, "Can't open data file: %s\n",data_file1);
    exit(-1);
  } 
  
  i=0;
  while( (n=fscanf(fp, "%lf %lf", &data[i][1], &data[i][2])) != EOF ){
    if( n == 2 ){
      data[i][0] = 1.0;
      i++;
    } 
    else {
      fprintf(stderr, "Something wrong in data1.\n");
      exit(-1);
    }
  }
  n_data1 = i;
  fclose(fp); 
  
  
  fp = fopen(data_file2, "r");
  if (fp == NULL){
    fprintf(stderr, "Can't open data file: %s\n",data_file2);
    exit(-1);
  } 
  
  while( (n=fscanf(fp, "%lf %lf", &data[i][1], &data[i][2])) != EOF ){
    if( n == 2 ){
      data[i][0] = 1.0;
      i++;
    }
    else {
      fprintf(stderr, "Something wrong in data2.\n");
      exit(-1);
    }
  }
  n_data = i;
  n_data2 = n_data - n_data1;
  fclose(fp); 
  
  
  gp = popen("gnuplot","w");
  fprintf(gp, "set xlabel \"x1\"\n");
  fprintf(gp, "set ylabel \"x2\"\n");
  fprintf(gp, "set xrange[%.2lf:%.2lf]\n",x_start,x_end);
  fprintf(gp, "set yrange[%.2lf:%.2lf]\n",y_start,y_end);
  fprintf(gp, "set size square\n");
  //      fprintf(gp, "set terminal postscript\n");
  
  
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
	    //		printf("t:%d\t pattern:%d\t z:%.5lf\t true:%d\t error:%.5lf\n", t, i, z, teacher, e);
	    
	    dw[0] = dw[0] + 2.0*eta*e*lambda*z*(1.0-z);
	    dw[1] = dw[1] + 2.0*eta*e*lambda*z*(1.0-z)*data[i][1];
	    dw[2] = dw[2] + 2.0*eta*e*lambda*z*(1.0-z)*data[i][2];
	  }
	  
	  /* comment out if you want to see something */
	  //		printf("t:%d \t e2:%.5lf\n", t, e2);
	  
	  /* update weights */	
	  w[0] = w[0] + dw[0];
	  w[1] = w[1] + dw[1];
	  w[2] = w[2] + dw[2]; 
	  
	  /* comment out if you want to see dynamics of weights */
	  //			printf("%d: %.5lf\t%.5lf\t%.5lf\n",t, dw[0],dw[1],dw[2]);
	  
	  /* plot at u=0 (hyper plane) */
	  fprintf(gp, "set title 't =%d'\n", t);
	  fprintf(gp, "plot '%s' with points pt 1, '%s' with points pt 3, %.3lf*x + %.3lf with lines\n", data_file1, data_file2, -w[1]/w[2], -w[0]/w[2]);
	  fflush(gp); 
	  
	  usleep(SLEEP);
	  
	}
	
}
