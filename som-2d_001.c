/*
  Self-Organizing Map (2D):
  2006.5.14  A.Date
  Last modified at 6 Aug 2020
  
  % gcc som-2d_001.c -lm 
  
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> /* for usleep */

#define N_XUNITS 10
#define N_YUNITS 10
#define DIM_INPUT 2 /* input signal size */

double X_RANGE=1.0;
double Y_RANGE=1.0;
int SLEEP=100000;
double ALPHA=0.2;
double SIGMA=0.8;
int N_LEARNING = 150;
int N_SUB_LEARNING = 100;

double S[2];    /* input */
double RV[N_XUNITS][N_YUNITS][DIM_INPUT]; /* reference vector */
FILE *gp;

void som2d();
void init_som();
void write_data();
double nrnd();

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
	int output_type = 0;
	
	for (i=1; i<argc; i++) {
		switch (*(argv[i]+1)) {
		case 'r':
			seed = atoi(argv[++i]);
			break;
		case 'f':
			output_type = atoi(argv[++i]);
			break;
		default:
			fprintf(stderr, "Usage : %s\n",argv[0]);
			fprintf(stderr, "\t-r : random-seed(%ld)\n",seed) ;
			fprintf(stderr, "\t-f : output type(%d)\n",output_type) ;
			exit(0);
			break;
		}
	}
	
	srand48(seed);
	
	gp = popen("gnuplot","w");
	
	if ( output_type == 1 ){
		fprintf(gp, "set terminal postscript \"Helvetica\" 20 color eps enhanced\n");
	}
	fprintf(gp, "set parametric\n");
	fprintf(gp, "set size ratio -1\n");
	fprintf(gp, "set style data linespoints\n");
	fprintf(gp, "set linetype 5 lc rgb \"light-red\" lw 3 pt 7 ps 1.0\n");
	fprintf(gp, "set xlabel \"x_1\"\n");
	fprintf(gp, "set ylabel \"x_2\"\n");
	fprintf(gp, "set xrange[0:1]\n");
	fprintf(gp, "set yrange[0:1]\n");
	fprintf(gp, "set nokey\n");
	fflush(gp);
	
	init_som();
	
	for(i=0;i<N_LEARNING;i++){
		
		if ( output_type == 1 ){  /* for making movies */
			fprintf(gp,"set output '| epstopdf -f -o=som2d_001_%d.pdf'\n",i);
		}
		fprintf(gp, "set title 't = %d/%d'\n", i*N_SUB_LEARNING, N_LEARNING*N_SUB_LEARNING);
		fprintf(gp, "plot '-' linestyle 5\n");
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


void init_som(){ 
	
	int i,j,k; 
	
	for (i=0; i < N_XUNITS; i++){
		for (j=0; j < N_YUNITS; j++){
			for (k=0; k<DIM_INPUT; k++){
				RV[i][j][k] = drand48();
			}
		}       
	}       
}


void som2d()
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

	// S[0] = nrnd()*X_RANGE + 0.5;
	// S[1] = nrnd()*Y_RANGE + 0.5;
    
	/* find the unit having the closest ref. vector to the input.*/
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
void write_data(){
	
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



/* generate random numbers from normal distribution */
double nrnd(){
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
