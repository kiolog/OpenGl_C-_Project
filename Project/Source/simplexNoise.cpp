// MIT License
//
// Copyright(c) 2017 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

// VERSION: 0.4.1

#ifndef _SIMPLEXNOISE_
#define _SIMPLEXNOISE_

#include <math.h>
#include <assert.h>

#include <algorithm>
#include <random>

class SimplexNoise
{
	int m_seed = 1337;
	unsigned char m_perm[512];
	unsigned char m_perm12[512];
	double m_gain = 0.5;
	double m_frequency =  1;
	double 	m_lacunarity = 2;
	int m_octaves = 3;
	double m_fractalBounding;

	const double GRAD_X[12] = { 1, -1, 1, -1, 1, -1, 1, -1, 0, 0, 0, 0 };
	const double GRAD_Y[12] = { 1, 1, -1, -1, 0, 0, 0, 0, 1, -1, 1, -1 };

	const double SQRT3 = double(1.7320508075688772935274463415059);
	const double F2 = double(0.5) * (SQRT3 - double(1.0));
	const double G2 = (double(3.0) - SQRT3) / double(6.0);

	double GradCoord2D(unsigned char offset, int x, int y, double xd, double yd) const {
		unsigned char lutPos = Index2D_12(offset, x, y);
		return xd * GRAD_X[lutPos] + yd * GRAD_Y[lutPos];
	}

	unsigned char Index2D_12(unsigned char offset, int x, int y) const {
		return m_perm12[(x & 0xff) + m_perm[(y & 0xff) + offset]];
	}

	double SingleSimplex(unsigned char offset, double x, double y) const {
		double t = (x + y) * F2;
		int i = std::floor(x + t);
		int j = std::floor(x + t);

		t = (i + j) * G2;
		double X0 = i - t;	double Y0 = j - t;
		double x0 = x - X0;	double y0 = y - Y0;

		int i1, j1;
		if (x0 > y0) {
			i1 = 1; j1 = 0;
		}
		else {
			i1 = 0; j1 = 1;
		}


		double x1 = x0 - (double)i1 + G2;
		double y1 = y0 - (double)j1 + G2;
		double x2 = x0 - 1 + 2 * G2;
		double y2 = y0 - 1 + 2 * G2;

		double n0, n1, n2;

		t = double(0.5) - x0 * x0 - y0 * y0;
		if (t < 0) n0 = 0;
		else {
			t *= t;
			n0 = t * t * GradCoord2D(offset, i, j, x0, y0);
		}

		t = double(0.5) - x1 * x1 - y1 * y1;
		if (t < 0) n1 = 0;
		else {
			t *= t;
			n1 = t * t*GradCoord2D(offset, i + i1, j + j1, x1, y1);
		}

		t = double(0.5) - x2 * x2 - y2 * y2;
		if (t < 0) n2 = 0;
		else {
			t *= t;
			n2 = t * t*GradCoord2D(offset, i + 1, j + 1, x2, y2);
		}

		return 70 * (n0 + n1 + n2) ;
	}


	double SingleSimplexFractalFBM(double x, double y) const
	{
		double sum = SingleSimplex(m_perm[0], x, y);
		double amp = 1;
		int i = 0;

		while (++i < m_octaves){
			x *= m_lacunarity;
			y *= m_lacunarity;

			amp *= m_gain;
			sum += SingleSimplex(m_perm[i], x, y) * amp;
		}

		return sum / m_fractalBounding;
	}

	void SetSeed(int seed) {
		m_seed = seed;

		mt19937_64 gen(seed);

		for (int i = 0; i < 256; i++)
			m_perm[i] = i;

		for (int j = 0; j < 256; j++) {
			int rng = (int)(gen() % (256 - j));
			int k = rng + j;
			int l = m_perm[j];
			m_perm[j] = m_perm[j + 256] = m_perm[k];
			m_perm[k] = l;
			m_perm12[j] = m_perm12[j + 256] = m_perm[j] % 12;
		}
	}
	void CalculateFractalBounding(){
		double amp = m_gain;
		double ampFractal = 1.0f;
		for (int i = 1; i < m_octaves; i++){
			ampFractal += amp;
			amp *= m_gain;
		}
		m_fractalBounding = ampFractal;
	}
public:
	SimplexNoise(int seed = 1337) { SetSeed(seed); CalculateFractalBounding();};
	~SimplexNoise() {};
	void SetFrequency(double frequency) { m_frequency = frequency; }
	double GetNoise(double x, double y) {
		// double noiseVal =  SingleSimplexFractalFBM(x * m_frequency, y * m_frequency);
		double noiseVal =  SingleSimplex(0 , x * m_frequency, y * m_frequency);
		return noiseVal;
	}
};
#endif
