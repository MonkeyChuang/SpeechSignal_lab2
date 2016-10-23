#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "wavfile.h"

#define rate 44100
#define dt (1./44100)
/*provide frequence(f) or period(T), and elapsed time(tTime) */
#define T (1./440)
#define totalTime 5

/*compute the output of the function define in fun(double x),
the input argument represents the value of the variable of the function
whose data type is double*/
double fun(double);

/*evaluate the first 'nMax' fourier coefficients of the function defined in
fun(double x). 't' stands for the period of the function, and
'length' stands for the length of array to stores the dependent
variables of the funtion in fun(double x). */
double** DFT_fun(double t,int length,int nMax);

int main()
{
    int length = totalTime*rate;
    int nMax;
    printf("provide an input for nMax > 0 (1~50 is recommended) :");
    scanf("%d",&nMax);
    while(nMax <= 0)
    {
        printf("invalid input. Try again\n");
        printf("nMax = (1~50 is recommended)");
        scanf("%d",&nMax);
    }

    FILE* f;
/*----create audio data y_fun[], sound_o.wav----*/
    short y_fun[length];
    for(int i=0;i<length;++i)
    {
        double x = (i%(int)(T*rate))*dt;
        y_fun[i] = fun(x);
    }

    f = wavfile_open("sound_o.wav");
    if(!f)
    {
        fprintf(stderr,"couldn't open sound.wav for writing.\n");
        return 1;
    }
    wavfile_write(f,y_fun,length);
    wavfile_close(f);

/*----Reconstruct the function with first nMax fourier coefficients.
create sound_m.wav----*/
    double** c = DFT_fun(T,length,nMax);

    short y_rectr1[length];
    double y=0;
    for(int i =0;i<length;++i)
    {
        y= *(*(c));
        for(int j=1;j<nMax;++j)
        {
            double x = i*dt;
            y += ( (*(*(c)+j))*cos(2*M_PI*j*(1/T)*x) + *(*(c+1)+j)*sin(2*M_PI*j*(1/T)*x) );
        }
        y_rectr1[i] = y;
    }

    f = wavfile_open("sound_n1.wav");
    if(!f)
    {
        fprintf(stderr,"couldn't open sound.wav for writing.\n");
        return 1;
    }
    wavfile_write(f,y_rectr1,length);
    wavfile_close(f);

/*----Reconstruct the function with nMax fourier coefficients regardless
 phase angleof sin(2pi*i*t/T) and cos(2pi*i*t/T)----*/
    short y_rectr2[length];
    y=0;
    for(int i =0;i<length;++i)
    {
        y= *(*(c));
        for(int j=1;j<nMax;++j)
        {
            double x = i*dt;
            y += ( sqrt(pow(*(*(c)+j),2)+pow(*(*(c+1)+j),2))*sin(2*M_PI*j*(1/T)*x));
        }
        y_rectr2[i] = y;
    }

    f = wavfile_open("sound_n2.wav");
    if(!f)
    {
        fprintf(stderr,"couldn't open sound.wav for writing.\n");
        return 1;
    }
    wavfile_write(f,y_rectr2,length);
    wavfile_close(f);

/*----Output a,b to file coef.txt----*/
    FILE* f_c;
    f_c = fopen("coef.txt","w");
    fprintf(f_c,"%d %f %d %d ",rate,T,totalTime,nMax);
    for(int i=0;i<2;++i)
    {
        for(int j=0;j<nMax;++j)
            fprintf(f_c,"%lf",*(*(c+i)+j));
    }

    return 0;
}

double fun(double t)
{
    double k = 4e22;
    double y = k*t*t*(t-T/4)*(t-T/2)*(t-T)*(t-T);
    return y;
}

double** DFT_fun(double t,int length,int nMax)
{
    double** c;
    double *a,*b;
    a = (double*)calloc(nMax,sizeof(double));
    b = (double*)calloc(nMax,sizeof(double));
    c = (double**)calloc(2,sizeof(double*));
    *(c) = a;
    *(c+1) = b;

    int i,j;
    for(i=0;i<nMax;++i)
    {
        for(j=0;j<t*rate;++j)
        {
            double x = (j+0.5)*dt;
            *(a+i) += fun(x)*cos(2*M_PI*i*(1/t)*x);
            *(b+i) += fun(x)*sin(2*M_PI*i*(1/t)*x);
        }
        *(a+i) = (*(a+i))*dt*2/t;
        *(b+i) = (*(b+i))*dt*2/t;
    }
    return c;
}
