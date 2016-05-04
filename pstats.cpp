#include <math.h>
//#include <stdlib.h>
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

class Continuous {
	public:
		virtual double quantile(double d) {
			return INT_MAX;
		}
		std::vector<double> random(int length) {
			std::vector<double> result;
			// between 1 and 0
			for (int j=0; j<length; j++) {
				double r = ((double) rand() / (RAND_MAX));
				result.push_back(quantile(r));
			}
			return result;
		}
		double random() {
			double r = ((double) rand() / (RAND_MAX));
			return quantile(r);
		}
		void seed(int s) {
			srand(s);
		}
		// seed when initialized
		Continuous() {
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

// Laplace
class Laplace: public Continuous {
	public:
		double pdf(double);
		double cdf(double);
		virtual double quantile(double);
		int random(){
			return Continuous::random();
		};
		std::vector<double> random(int length) {
			return Continuous::random(length);
		};
		// mean and scale parameter, respectively
		Laplace(double m, double b)
			: m_m(m), m_b(b)
		{}
		double m_m;
		double m_b;
};

double Laplace::pdf(double x) {
	return exp(-fabs(double(x - m_m))/m_b)/2;
}
double Laplace::cdf(double x) {
	if (x < m_m) {
		return exp(double(x - m_m)/m_b)/2;
	}
	else if (x >= m_m) {
		return 1 - exp((m_m - x)/m_b)/2;
	}
	else {
		// wrong value for input; not ideal treatment,
		// but -1 is an obviously invalid response
		return -1;
	}
}

double Laplace::quantile(double p) {
	if (p > 0 and p <= 0.5) {
		return m_m + m_b*log(2*p);
	}
	else if (p > 0.5 and p < 1) {
		return m_m - m_b*log(2*(1-p));
	}
	return -1;
}

void dump(std::vector<int> data) {
	for (std::vector<int>::const_iterator i = data.begin(); i != data.end(); ++i)
		std::cout << *i << '\n';
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
//	for (std::vector<int>::const_iterator i = d.begin(); i != d.end(); ++i)
//		std::cout << *i << '\n';
	dump(d);
	Laplace l = Laplace(0.0, 1.0);
	printf("l's pdf of 1 is %f \n", l.pdf(1)); //0.183940
	printf("l's cdf of 1 is %f \n", l.cdf(1)); //0.816060
}
