/*
    frk.cpp
    11/09/15
    Chris Lacher

    driver for class RabinKarp<128>
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <rk.h>
#include <ansicodes.h>

/* 
   n        largest prime <= n
   ----     ------------------
   2^32     4294967291
   2^31     2147483647
   2^30     1073741789
   2^29      536870909
   2^28      268435399
*/

const size_t alphabet_size = 128;
const size_t prime = 4294967291; // = fsu::PrimeBelow(0xFFFFFFFF);

void Align (const char* s, const char* p, size_t offset, std::ostream& os = std::cout);

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << " ** arguments:\n"
              << "    1: string \'pattern\'   (required)\n"
              << "    2: string \'text\'      (required)\n"
              << "    3: int        {0 = silent, 1 = proof, 2 = dump} (optional)\n"
              << "    4: bool vegas {0 = Monte Carlo, 1 = Las Vegas}  (optional)\n"
              << " *** try again\n";
    return EXIT_FAILURE;
  }
  unsigned short verbosity = 0;
  bool proof = 0, dump = 0;
  if (argc > 3)
  {
    verbosity = atoi(argv[3]);
    if (verbosity > 0)
      proof = 1;
    if (verbosity > 1)
      dump = 1;
  }
  bool vegas = 0;
  if (argc > 4)
  {
    vegas = atoi(argv[4]);
  }
  char* p = argv[1];
  char* s = argv[2];
  RabinKarp<alphabet_size, prime> rk;

  rk.Init(p);
  size_t loc =  rk.Search(s, vegas);
  std::cout << " RabinKarp::Search result: " << loc << '\n';
  if (proof) Align (s,p,loc);
  if (dump)  rk.Dump();
}

void Align (const char* s, const char* p, size_t offset, std::ostream& os)
{
  size_t n = strlen(s), m = strlen(p);
  // output s
  os << ' ';
  if (offset < n)
  {
    for (size_t i = 0; i < offset; ++i) os << s[i];
    os << ANSI_REVERSE;
    for (size_t i = offset; i < offset + m; ++i) os << s[i];
    os << ANSI_RESET_ALL;
    for (size_t i = offset + m; i < n; ++i) os << s[i];
    os << '\n';
  }
  else
  {
    os << s << '\n';
  }
  // output p in offset position
  os << ' ';
  for (size_t i = 0; i < offset; ++i) os << ' ';
  os << ANSI_REVERSE;
  os << p << '\n';
  os << ANSI_RESET_ALL;
}

