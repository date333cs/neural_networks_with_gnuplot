/* 
	Backpropagation demo by gnuplot 
	
	2006.5.18 A.Date
	Last modified at 14 Apr 2016
 */

#include <stdio.h>
#include <stdlib.h> /* drand48 */
#include <math.h> /* log */
#include <unistd.h> /* for usleep */

#define MAX_UNITS 100
#define MAX_DATA  1000
#define N_DIM 3 /* (dimension of input) + 1 */

/* 以下のパラメータ値を変えて試してみる */

#define RAND_SEED  12345678  /* 乱数の種 */
#define SLEEP 100000   /* 表示スピード */
int n_hidden = 3;   /* 中間層の素子の数 */
int n_trials = 200; /* 学習回数 */
double eta = 0.1; /* 学習係数 */
double lambda=1.0; /* シグモイド関数のパラメータ */
char data_file1[] = "apples3.dat";
char data_file2[] = "oranges3.dat";
char error_file[] = "errors3.dat";

int n_data, n_data1, n_data2;

/* a random sample from standard normal distribution */
/* modified from C 言語による最新アルゴリズム辞典 p.135 */
double nrnd()
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


double sigmoid(double x){
  
  return 1.0/(1.0 + exp(-lambda*x));
}


int
main (int argc, char *argv[])
{
  FILE *gp, *gp2, *fp, *fp2; 
  int a,i,j,k,l,n,t;
  long int seed = 1234523890;  /* set your favorite number */
  int n_data;
  int teacher, answer;
  int e3;
  
  double d1,d2;
  double dw[MAX_UNITS][N_DIM];  
  double w[MAX_UNITS][N_DIM]; /* w[i][j], y_1,...y_i  */
  double data[MAX_DATA][N_DIM]; 
  double u[MAX_UNITS], ui, v, z, e, e2, eta2;
  double r0, r[MAX_UNITS];
  double y[MAX_UNITS]; /* output of hidden units */
  double s[MAX_UNITS]; /* connection weights of output unit */
  double ds[MAX_UNITS]; 
  double x_start = -1.0, x_end=5.0;
  double y_start = -3.0, y_end=5.0; 
  
  for (i=1; i<argc; i++) {
    switch (*(argv[i]+1)) {
    case 'r':
      seed = atoi(argv[++i]);
      break;
    default:
      fprintf(stderr, "Usage : %s\n",argv[0]);
      fprintf(stderr, "\t-r : random-seed(%ld)\n",seed);
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
  
  gp = popen("gnuplot -geometry 640x480+0+0","w");
  fprintf(gp, "set term x11\n");
  fprintf(gp, "set xlabel \"x1\"\n");
  fprintf(gp, "set ylabel \"x2\"\n");
  fprintf(gp, "set xrange[%.2lf:%.2lf]\n",x_start,x_end);
  fprintf(gp, "set yrange[%.2lf:%.2lf]\n",y_start,y_end);
  fprintf(gp, "set size square\n");
  
  gp2 = popen("gnuplot -geometry 640x480-0+0","w");
  fprintf(gp2, "set term x11\n");
  fprintf(gp2, "set parametric\n");
  fprintf(gp2, "set xlabel 'x1'\n");
  fprintf(gp2, "set ylabel 'x2'\n");
  fprintf(gp2, "set xrange[%.2lf:%.2lf]\n",x_start,x_end);
  fprintf(gp2, "set yrange[%.2lf:%.2lf]\n",y_start,y_end);
  fprintf(gp2, "set size square\n");
  
  for (l=0; l<10; l++) { /* デモを10回繰り返す */
    
    /* 回路の初期設定 */
    for (i=0; i<= n_hidden; i++) {
      s[i] =  nrnd();
      for (j=0; j< N_DIM; j++) {
	w[i][j] = nrnd();
      }
    }
    
    /* loop over trainign set */
    for (t=0; t<n_trials; t++) { 
      
      for (i=0; i<= n_hidden; i++) {
	ds[i] = 0.0;
	for (j=0; j< N_DIM; j++) {
	  dw[i][j] = 0.0;
	}
      }		
      e2 = 0.0;
      eta2 = 2.0*eta*lambda*lambda;
      y[0] = 1.0;

      e3=0;
      for ( k=0; k<n_data; k++) { 
	v = s[0];
	for ( i=1; i<= n_hidden; i++) {
	  u[i] = w[i][0] + w[i][1]*data[k][1] +  w[i][2]*data[k][2];
	  y[i] = sigmoid(u[i]);
	  v = v + s[i]*y[i];
	}
	z = sigmoid(v);
	
	if ( k < n_data1 ){
	  teacher = 1;
	}
	else{
	  teacher = 0;
	}
	
	if ( z > 0.5){
	  answer = 1;
	}
	else{
	  answer = 0;
	}
	
	e3 += (teacher - answer)*(teacher - answer);
	
	e = (double)teacher - z;
	e2 = e2 + e*e;
	
	r0 = -2.0*lambda*e*z*(1.0-z);
	
	for ( i=0; i<= n_hidden; i++) {
	  ds[i] = ds[i] - eta*r0*y[i];
	}
	
	for ( i=1; i<n_hidden+1; i++) {
	  r[i] = r0*s[i]*lambda*y[i]*(1.0-y[i]);
	  dw[i][0] = dw[i][0] - eta*r[i];
	  dw[i][1] = dw[i][1] - eta*r[i]*data[k][1];
	  dw[i][2] = dw[i][2] - eta*r[i]*data[k][2];
	}
      } 
      
      printf("t:%d\t e: %.5lf \t error: %d/%d\n", t,e2, e3, n_data);
      
      /* draw area */
      if ( t % 10 ==0 ){ /* 10回に1回, 画面を更新*/
	fp2 = fopen(error_file, "w"); 
	for ( d1=x_start; d1<x_end; d1=d1+0.05) {
	  for ( d2=y_start; d2<y_end; d2=d2+0.05) {
	    v = s[0];
	    y[0] = 1.0;
	    for ( i=1; i<= n_hidden; i++) {
	      u[i] = w[i][0] + w[i][1]*d1 +  w[i][2]*d2;
	      y[i] = sigmoid(u[i]);
	      v = v + s[i]*y[i];
	    }
	    z = sigmoid(v);
	    if ( z > 0.5 ){ 
	      fprintf(fp2, "%.3lf \t %.3lf\n", d1, d2);
	    }	
	  }
	}		
	fflush(fp2); 
	fclose(fp2); 
	
	fprintf(gp2, "plot '%s' with points pt 1, '%s' with points pt 3,", data_file1, data_file2);
	if ( e3==0 ){
	  fprintf(gp2, "'%s' with dots dt 5\n",error_file);
	}
	else{
	  fprintf(gp2, "'%s' with dots dt 72\n",error_file);
	}
	fflush(gp2); 
	
      }
      
      /* update weights */	
      s[0] = s[0] + ds[0];
      for ( i=1; i<=n_hidden; i++) {
	s[i] = s[i] + ds[i];
	w[i][0] = w[i][0] + dw[i][0];
	w[i][1] = w[i][1] + dw[i][1];
	w[i][2] = w[i][2] + dw[i][2];
      }
      
      /* plot at u=0 (hyper planes) */
      fprintf(gp, "set title 't =%d, error:%3d/%3d'\n", t,e3,n_data);
      fprintf(gp, "plot '%s' with points pt 1, '%s' with points pt 3", data_file1, data_file2);
      for ( i=1; i<=n_hidden; i++) {
	fprintf(gp, ",%.3lf*x + %.3lf with lines ", -w[i][1]/w[i][2], -w[i][0]/w[i][2]);
	if ( w[i][2] > 0 ){
	  fprintf(gp, "lt 1");
	}
	else {
	  fprintf(gp, "lt 3");
	}
      }
      fprintf(gp,"\n");
      fflush(gp); 
      
      usleep(SLEEP);
      
    } /* end of a trial */
  }
  
}
