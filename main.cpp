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

double simpson(int i, int k,double a, double b, double (*f) (int, int, double))
{
	double h = 4.5;
	double soma = f(i,k,a) + f(i,k,b);
	int w = 0;
	double j = a;

	while(1)
	{
		j += h;
		if(j > b - 1)
			break;
		if(w%2 == 0)
			soma += f(i,k,j) *2;
		else
			soma += f(i,k,j) *4;

		w++;
	}

	return (h/3) * soma; 
}


double tr(double s)
{
	if(s < 0.3)
	{
		return 0.0;
	}
	else
	{
		return (0.05 * (s - 0.3));
	}
}

double f2(int i, int k, double j)
{
	int j0 = floor(j);
	int j1 = j0 + 1;

	int d0 = v[k*nx*ny+j0*nx+i];
	int d1 = v[k*nx*ny+j1*nx+i];

	double d = d0 + (d1 - d0) * (j- j0) / (j1 - j0);

	d /= 255.0;

	return tr(d);
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
			double d1 = simpson(2 * i, k, 0, 255.0, f1);
			double d2 = simpson((2* i)+ 1, k, 0, 255.0, f1);
			
			int d = (int) (((d1 + d2) / 2 )* 255.0);

			if(d > maior)
				maior = d;

			densidade[i][k] = d;
		}
	
	}

	out << maior << "\n";

	for(int i =0 ; i< 128; i++)
	{
		for(int k = 0; k < 99; k++)
		{
			out << densidade[i][k] << " ";
		}
		out << "\n";
	}

	fclose(f);
	out.close();
}