/*
   Associative Memory 
   2006.5.26  A.Date
   Last modified 14 Apr 2016
*/

#include <stdlib.h>  /*  drand48() */
#include <stdio.h>
#include "kanji1.h"
#include <unistd.h> /* for usleep */

#define MAX_X 20
#define MAX_Y 20
#define MAX_UNITS 400

/* Change the following values */
int RAND_SEED = 12345678;

int N_SUPERIMPOSED = 8;
double NOISE_LEVEL = 0.4;

/* set this value according to your data */
int N_XUNITS = 20;
int N_YUNITS = 20;
int N_UNITS = 400; /* shoud be  N_XUNITS*N_YUNITS */

int N_DYNAMICS = 10; 
int N_ITERATE = 10;  /* #demonstrations */

/* if too slow or too fast, change this */
int SLEEP1 =  500000;
int SLEEP = 1000000;

void write_data();
void init_state(int i);
void add_noise();
void init_connection();
void dynamics(int i);
void backup_state();
double inner_product(int k);

FILE *gp1, *gp2;

double W[MAX_UNITS][MAX_UNITS];
int X[MAX_UNITS];  
int BX[MAX_UNITS];  

int main (int argc, char *argv[] )
{ 
  int i,j,k;
  int p;
  double x,y;
  double dc;
  double noise_prob;
  long seed = RAND_SEED; 
  
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
  init_connection();
  
  gp1 = popen("gnuplot -geometry 480x480","w");
  
  /*
    gp1 = popen("gnuplot","w");
    fprintf(gp1, "set terminal postscript eps color \"Times\" 20\n");
  */
  fprintf(gp1, "set term x11\n");
  fprintf(gp1, "set pm3d\n");
  fprintf(gp1, "set view 0,0\n");
  //	fprintf(gp1, "set view 15,30\n"); 
  //	fprintf(gp1, "set view 5,5\n"); 
  fprintf(gp1, "unset key\n");
  
  gp2 = popen("gnuplot -geometry 480x360","w");
  fprintf(gp2, "set term x11\n");
  fprintf(gp2, "set style data lines\n");
  fprintf(gp2, "set xlabel \"time\"\n");
  fprintf(gp2, "set ylabel \"inner product\"\n");
  fprintf(gp2, "set xrange[:]\n");
  fprintf(gp2, "set yrange[-1:1.1]\n");
  fprintf(gp2, "set nokey\n");
  
  
  for (k=0; k<N_ITERATE; k++) {
    
    p = (int)(drand48()*N_SUPERIMPOSED);  /* select a pattern */
    init_state(p);
    noise_prob = NOISE_LEVEL;  
    //		noise_prob = drand48()/2.0;  
    
    add_noise(noise_prob);  
    backup_state();
    
    fprintf(gp2, "plot '-' with lines\n");
    
    for(i=0; i <N_DYNAMICS; i++){
      //			fprintf(gp1, "set out 'filename%d-%d.eps'\n",k,i);
      fprintf(gp1, "splot '-' with pm3d\n");
      write_data(); 
      fprintf(gp1,"e\n");
      
      dc = inner_product(p); 
      fprintf(gp2, "%d  %.5lf\n", i, dc);
      fflush(gp2);
      
      /* comment out if you want to see direction cosines */
      //			printf("%d  %.5lf\n", i, dc); 
      
      fprintf(gp1, "set title \"noise=%.3lf, t=%d, p=%d, dc=%.3lf\"\n",noise_prob,i,p, dc);
      fflush(gp1); 
      
      for(j=0;j<N_UNITS; j++){
	dynamics( j );
      }
      backup_state();
      
      usleep(SLEEP1);
    }
    fprintf(gp2,"e\n");
    fflush(gp2);
    usleep(SLEEP);
  }
  fclose(gp1);
}


void
init_state(int k){
  
  int i;
  for (i=0; i<N_UNITS; i++){
    if ( P[k][i] == 1 ){
      X[i] = 1;
    }		
    else{
      X[i] = -1;
    }		
  }
}

void
backup_state(){
  
  int i;
  for (i=0; i<N_UNITS; i++)
    BX[i] = X[i];
}


void init_connection(){
  int i,j,k;
  int tmp;
  
  for (i=0; i<N_UNITS; i++){
    W[i][i]=0.0;
    for (j=i+1; j<N_UNITS; j++){
      tmp=0;
      for (k=0; k<N_SUPERIMPOSED;  k++){
	if ( P[k][i] == P[k][j] ){
	  tmp++;
	}
	else{
	  tmp = tmp - 1;
	}
      }
      W[i][j]=(double)tmp/(double)N_UNITS;
      W[j][i]=(double)tmp/(double)N_UNITS;
    }
  }
}


void
dynamics(int i){
  int j;
  double u;
  
  u = 0.0;
  for (j=0; j<N_UNITS; j++){
    u = u + W[i][j]*(double)BX[j]; 
  }
  if ( u > 0.0){
    X[i] = 1;
  }
  else{
    X[i] = -1;
  }
  
}


double
inner_product(int k){
  
  int i;
  int c = 0;
  
  for (i=0; i<N_UNITS; i++){
    if ( (X[i] == 1 && P[k][i] == 1) ||  (X[i] == -1 && P[k][i] == 0)){
      c++;
    }		
    else{
      c--;
    }
  }
  
  return ((double)c/(double)N_UNITS);
}


void
add_noise(double noise){
  
  int i;
  for (i=0; i<N_UNITS; i++){
    if ( drand48() < noise ){
      if ( X[i] == 1 ){
	X[i] = -1;
      }
      else{
	X[i] = 1;
      }
    }
  }
  
}


void
write_data(){ 
  
  /* write data for gnuplot */
  
  int i,j;
  
  for (j=0; j<N_XUNITS; j++){
    for (i=0; i<N_YUNITS; i++){
      fprintf(gp1, "%d  %d %d\n", j, i,    X[j+(N_YUNITS-i-1)*N_XUNITS]);
      fprintf(gp1, "%d  %d %d\n", j, i+1,  X[j+(N_YUNITS-i-1)*N_XUNITS]); 
      
    }
    fprintf(gp1, "\n"); 
    
    for (i=0; i<N_YUNITS; i++){
      fprintf(gp1, "%d  %d %d\n", j+1,   i, X[j+(N_YUNITS-i-1)*N_XUNITS]);
      fprintf(gp1, "%d  %d %d\n", j+1, i+1, X[j+(N_YUNITS-i-1)*N_XUNITS]);
    }
    fprintf(gp1, "\n"); 
  }               
  
}
