#include <gmp.h>
#include <mpfr.h>

#include "arb_prec.hpp"

long arb_prec_t::precision_print = 7;

arb_prec_t::arb_prec_t(void) {
	mpfr_init2(v, PRECISION);
	mpfr_set_d(v, 0.0, MPFR_RNDN);
}

arb_prec_t::arb_prec_t(double val) {
	mpfr_init2(v, PRECISION);
	mpfr_set_d(v, val, MPFR_RNDN);
}

arb_prec_t::~arb_prec_t() {
	mpfr_clear(v);
	mpfr_free_cache();
}

arb_prec_t& arb_prec_t::operator=(double num) {
	mpfr_set_d(v, num, MPFR_RNDN);
	return *this;
}

const arb_prec_t arb_prec_t::operator+ (const arb_prec_t &b) {
	return arb_prec_t(*this) += b;
}

const arb_prec_t arb_prec_t::operator+ (const double b) {
	return arb_prec_t(*this) += b;
}

arb_prec_t& 	 arb_prec_t::operator+=(const arb_prec_t &b) {
	mpfr_add(v, v, b.v, MPFR_RNDN);
	return *this;
}

arb_prec_t& 	 arb_prec_t::operator+=(const double b) {
	mpfr_add_d(v, v, b, MPFR_RNDN);
	return *this;
}


const arb_prec_t arb_prec_t::operator- (const arb_prec_t &b) {
	return arb_prec_t(*this) -= b;
}

const arb_prec_t arb_prec_t::operator- (const double b) {
	return arb_prec_t(*this) -= b;
}

arb_prec_t& 	 arb_prec_t::operator-=(const arb_prec_t &b) {
	mpfr_sub(v, v, b.v, MPFR_RNDN);
	return *this;
}

arb_prec_t& 	 arb_prec_t::operator-=(const double b) {
	mpfr_add_d(v, v, b, MPFR_RNDN);
	return *this;
}


const arb_prec_t arb_prec_t::operator* (const arb_prec_t &b) {
	return arb_prec_t(*this) *= b;
}

const arb_prec_t arb_prec_t::operator* (const double b) {
	return arb_prec_t(*this) *= b;
}

arb_prec_t& 	 arb_prec_t::operator*=(const arb_prec_t &b) {
	mpfr_mul(v, v, b.v, MPFR_RNDN);
	return *this;
}

arb_prec_t& 	 arb_prec_t::operator*=(const double b) {
	mpfr_mul_d(v, v, b, MPFR_RNDN);
	return *this;
}

const arb_prec_t arb_prec_t::operator/ (const arb_prec_t &b) {
	return arb_prec_t(*this) /= b;
}

const arb_prec_t arb_prec_t::operator/ (const double b) {
	return arb_prec_t(*this) /= b;
}

arb_prec_t& 	 arb_prec_t::operator/=(const arb_prec_t &b) {
	mpfr_div(v, v, b.v, MPFR_RNDN);
	return *this;
}

arb_prec_t& 	 arb_prec_t::operator/=(const double b) {
	mpfr_div_d(v, v, b, MPFR_RNDN);
	return *this;
}

void arb_prec_t::set_precision_print(long precision) {
	arb_prec_t::precision_print = precision;
}

std::ostream& operator<<(std::ostream& os, const arb_prec_t &num)
{
	char* buff = nullptr;
	char precision[sizeof("%%.%ldRg") + 20];
	sprintf(precision, "%%.%ldRg", num.precision_print);
	mpfr_asprintf(&buff, precision, num.v);
	if (buff) {
		// buff[exponent] = '\0';
		os << buff;
	} else
		os << "[failed to get number]";
	mpfr_free_str(buff);
	return os;
}