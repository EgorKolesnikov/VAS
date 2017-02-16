#include <fftw3.h>
#include <iostream>

int main(){
	int N = 9;
 
	fftw_complex *in, *out;
	in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
	 
	in[0][0] = 1;
	in[0][1] = 0;
	in[1][0] = 2;
	in[1][1] = 0;
	in[2][0] = 3;
	in[2][1] = 0;
	in[3][0] = 4;
	in[3][1] = 0;
	in[4][0] = 5;
	in[4][1] = 0;
	in[5][0] = 6;
	in[5][1] = 0;
	in[6][0] = 7;
	in[6][1] = 0;
	in[7][0] = 8;
	in[7][1] = 0;
	in[8][0] = 9;
	in[8][1] = 0;
	 
	fftw_plan my_plan;
	my_plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(my_plan);
	  
	for(int i = 0; i < N; ++i) {
		std::cout << out[i][0] << " " << out[i][1] << "\n";
	} 
	 
	fftw_destroy_plan(my_plan);
	fftw_free(in);
	fftw_free(out);
    return 0;
}