#include <stdlib.h>
#include <iostream>
#include <climits>
#include <vector>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define rdtsc __rdtsc
#else
unsigned long long rdtsc(){
	unsigned int lo,hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((unsigned long long)hi << 32) | lo;
}
#endif

class Discrete {
	public:
		virtual int quantile(double d) {
			return INT_MAX;
		}
		std::vector<int> random(int length) {
			std::vector<int> result;
			// between 1 and 0
			for (int j=0; j<length; j++) {
				double r = ((double) rand() / (RAND_MAX));
				result.push_back(quantile(r));
			}
			return result;
		}
		int random() {
			double r = ((double) rand() / (RAND_MAX));
			return quantile(r);
		}
		void seed(int s) {
			srand(s);
		}
		// seed when initialized
		Discrete() {
			seed(rdtsc());
		}
};

class Bernoulli: public Discrete {
	public:
		double pmf(int);
		double cdf(int);
		virtual int quantile(double);
		int random(){
			return Discrete::random();
		};
		std::vector<int> random(int length) {
			return Discrete::random(length);
		};
		Bernoulli(double p)
			: m_p(p)
		{}
		double m_p;
};

double Bernoulli::pmf(int k) {
	if (k == 1) {
		return m_p;
	}
	else if (k == 0) {
		return 1 - m_p;
	}
	return -1;
}

double Bernoulli::cdf(int k) {
	if (k < 0) {
		return 0;
	}
	else if (k < 1) {
		return 1 - m_p;
	}
	else if (k >= 1) {
		return 1;
	}
	return -1;
}

int Bernoulli::quantile(double p) {
	if (p < 0) {
		return -1;
	}
	else if (p < 1 - m_p) {
		return 0;
	}
	else if (p <= 1) {
		return 1;
	}
	return -1;
}

int main() {
	// Examples
	Bernoulli b = Bernoulli(0.5);
	printf("b is %f \n", b.m_p); //0.5
	printf("b's pmf of 1 is %f \n", b.pmf(1)); //0.5
	printf("b's cdf of 1 is %f \n", b.cdf(1)); //1.0
	printf("b's quantile of 1 is %d \n", b.quantile(1.0)); // 1
	//srand(0);
	printf("b's random is %d \n", b.random()); //0
	std::vector<int> d = b.random(10);
	for (std::vector<int>::const_iterator i = d.begin(); i != d.end(); ++i)
		    std::cout << *i << '\n';
}
