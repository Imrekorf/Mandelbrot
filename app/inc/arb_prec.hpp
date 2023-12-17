#pragma once
#include <ostream>

#include <gmp.h>
#include <mpfr.h>

class arb_prec_t {
	static constexpr mpfr_prec_t PRECISION = 200;

	mpfr_t v;
	static long precision_print;
public:

	arb_prec_t(void);
	arb_prec_t(double val);
	~arb_prec_t();

	static void set_precision_print(long precision);

	arb_prec_t& 	 operator=(double num);

	const arb_prec_t operator+ (const arb_prec_t &b);
	const arb_prec_t operator+ (const double b);
	arb_prec_t& 	 operator+=(const double b);
	arb_prec_t& 	 operator+=(const arb_prec_t &b);

	const arb_prec_t operator- (const arb_prec_t &b);
	const arb_prec_t operator- (const double b);
	arb_prec_t& 	 operator-=(const double b);
	arb_prec_t& 	 operator-=(const arb_prec_t &b);

	const arb_prec_t operator* (const arb_prec_t &b);
	const arb_prec_t operator* (const double b);
	arb_prec_t& 	 operator*=(const double b);
	arb_prec_t& 	 operator*=(const arb_prec_t &b);

	const arb_prec_t operator/ (const double b);
	const arb_prec_t operator/ (const arb_prec_t &b);
	arb_prec_t& 	 operator/=(const double b);
	arb_prec_t& 	 operator/=(const arb_prec_t &b);
	friend std::ostream& operator<<(std::ostream& os, const arb_prec_t& dt);
};