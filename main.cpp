#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>

using namespace std;

int nx = 256;
int ny = 256;
int nz = 99;
int n = nx*ny*nz;
unsigned char* v = (unsigned char*)malloc(n*sizeof(unsigned char));

double simpson(int i, int k, double a, double b, double (*f) (int, int, double))
{
	double h = b - a;
    return (h / 6) * (f(i,k,a) + 4 * f(i,k,(a + b) / 2) + f(i,k,b));
}


double CompositeSimpson(int i, int k,double start, double end, double (*f) (int, int, double))
{
	double a = start;
    double b = 4.5;
    double sum = 0;

    while (b < end) {
        sum += simpson(i,k,a, b, f);
        a = b;
        b = b + 4.5;
    }
    if (a < end)
        sum += simpson(i,k,a, end, f);

    if(sum > 1)
    	return 1;
    else
    	return sum;
}

double f2(int i, int k, double j)
{
	int j0 = floor(j);
	int j1 = j0 + 1;

	int d0 = v[k*nx*ny+j0*nx+i];
	int d1 = v[k*nx*ny+j1*nx+i];

	double d = d0 + (d1 - d0) * (j- j0) / (j1 - j0);

	d /= 255.0;

	if(d < 0.3)
	{
		return 0.0;
	}
	else
	{
		return (0.05 * (d - 0.3));
	}

}

double f1(int i, int k, double j)
{
	return f2(i,k,j) * exp(-simpson(i,k,0, j, f2));
}


int main()
{
	FILE* f = fopen("head-8bit.raw","rb");
	ofstream out;
	out.open("final.pgm");
	int densidade[128][99];
	if (!f || !out) {
	   printf("Erro ao abrir o arquivo\n");
	   return 1;
	}
	int i = 0;
	if (fread(v,sizeof(unsigned char),n,f) != n) {
	   printf("Erro na leitura do arquivo\n");
	   return 1;
	}

	out << "P2\n128 99\n";


	int maior =0;

	for(int i = 0; i < 128; i++)
	{
		for(int k = 0; k < 99; k++)
		{
			double d1 = CompositeSimpson(2 * i, k, 0, 255.0, f1);
			double d2 = CompositeSimpson((2* i)+ 1, k, 0, 255.0, f1);
			
			double d = (d1 + d2) / 2;

			d*= 255;

			if(d > maior)
				maior = d;

			densidade[i][k] = d;
		}
	
	}

	out << maior << "\n";

	for(int k = 0; k < 99; k++)
	{
		for(int i =0 ; i< 128; i++)
		{
			out << densidade[i][k] << " ";
		}
		out << "\n";
	}

	fclose(f);
	out.close();
}