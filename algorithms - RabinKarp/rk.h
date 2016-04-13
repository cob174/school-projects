#ifndef _RK_H
#define _RK_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ansicodes.h>

template <size_t R, size_t P> // alphabet size,  prime number
class RabinKarp
{
	public:
		RabinKarp()              : pattern_(nullptr), plength_(0), alength_(R), pathash_(0), prime_(P), RM_(1) {}
		RabinKarp(const char* p) : pattern_(nullptr), plength_(0), alength_(R), pathash_(0), prime_(P), RM_(1) { Init(p); }
		void   Init   (const char* p);
		size_t Search (const char* s, bool vegas = 0) const;
		void   Dump   (std::ostream& os = std::cout)  const;
		long double Probability() const;

	private: // data
		char*    pattern_;  // p = pattern
		uint64_t plength_;  // m = length of patternn
		uint64_t alength_;  // R = size of alphabet
		uint64_t pathash_;  // hash value of pattern
		uint64_t prime_;    // Q = prime divisor used in hash function
		uint64_t RM_;       // R^{m-1} % Q

	private: // methods
		uint64_t Hash   (const char* s, size_t length) const;
		bool     Verify (const char* s, size_t loc)    const;
};
	
template <size_t R, size_t P>
void RabinKarp<R, P>::Init (const char* p)
{
	unsigned int len = strlen(p);
	delete pattern_;
	pattern_ = new char[len];
	strcpy(pattern_, p);
	plength_ = strlen(pattern_);
	pathash_ = Hash(p, plength_);
	for (size_t i = 1; i < plength_; ++i)
		RM_ = (RM_ * alength_) % prime_;
	
}
	
template <size_t R, size_t P>
size_t RabinKarp<R, P>::Search (const char* s, bool vegas) const
{
	uint64_t txthash = Hash(s, plength_);
	if (txthash == pathash_)
	{
		if (!vegas || Verify(s,0)) return 0;
	}
	
	for (size_t i = plength_; i < strlen(s); ++i)
	{
		txthash = (txthash + prime_ - ((RM_*(uint64_t)s[i-plength_]) % prime_)) % prime_;
		txthash = (txthash * alength_ + (uint64_t)s[i]) % prime_;
		if (txthash == pathash_)
		{
			if (vegas && !Verify(s, i-plength_+1)) continue;
			return i-plength_+1;
		}
	}
	return strlen(s);
}
	
template <size_t R, size_t P>
void RabinKarp<R, P>::Dump (std::ostream& os) const
{
	os << "pattern:\t   " << pattern_ << '\n';
	os << "plength:\t" << plength_ << '\n';
	os << "R:\t\t" << alength_ << '\n';
	os << "pathash:\t" << pathash_ << '\n';
	os << "prime:\t\t" << prime_ << '\n';
	os << "RM:\t\t" << RM_ << '\n';
	os << "Probability:\t" << Probability() << '\n';
}
	
template <size_t R, size_t P>
long double RabinKarp<R, P>::Probability() const
{
	return 1.f/prime_;
}
	
template <size_t R, size_t P>
uint64_t RabinKarp<R, P>::Hash (const char* s, size_t length) const
{
	uint64_t hash = 0;
	for (size_t i = 0; i < length; ++i)
		hash = (alength_ * hash + (uint64_t)s[i]) % prime_;
	return hash;
}
	
template <size_t R, size_t P>
bool RabinKarp<R, P>::Verify (const char* s, size_t loc)    const
{
	for (size_t i = 0; i < plength_; ++i)
	{
		if(pattern_[i] != s[i+loc])
		{
			std::cout << " ** RK: match verification failure at s[" << loc << "]\n";
			return 0;
		}
	}
	std::cout << " ** RK:: match verified\n";
	return 1;
}
#endif