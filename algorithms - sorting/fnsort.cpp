/*
    fnsort.cpp
    09/17/13
    Chris Lacher

    functionality test of numerical sort algorithms

    Copyright 2013, R.C. Lacher
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>

#include <vector.h>
#include <genalg.h>
#include <gheap.h>
#include <nsort.h>
#include <compare.h>
#include <insert.h>

const char* e_t = "int";  const char ofc = ' ';

// these are used mainly in testing bit_sort and byte_sort
typedef signed char TinyElementType;    // 1
typedef short       SmallElementType;   // 2
typedef int         MediumElementType;  // 4
typedef long        LargeElementType;   // 8

namespace fsu
{

  // these are some function classes to test the 5-parameter version of
  // counting_sort

  class LowerBound
  // use to save processing space when numerical data is >= lowerBound
  {
  public:
    LowerBound(int lowerBound) : lowerBound_(lowerBound) {}
    size_t operator () (int i) { return static_cast<size_t>(i-lowerBound_); }
  private:
    int lowerBound_;
  } ;

  class Reverse
  // use to sort in reverse order
  {
  public:
    Reverse (int upperBound) : upperBound_(upperBound) {} 
    size_t operator () (int n) { return upperBound_ - n; }
  private:
    int upperBound_;
  };

  /*  // moved into nsort.h
  // this class facilitates bit_sort and is now defined in nsort.h
  // N can be any type that is convertible to an integer type
  // note that bit_sort is a template function taking this same template parameter
  template <typename N>
  class Bit
  {
  public:
    size_t operator () (N n)
    {
      return (0 != (mask_ & n));
    }
    Bit() : mask_(static_cast<N>(0x01)) {}
    void SetBit(unsigned char i) { mask_ = (static_cast<N>(0x01) << i); }
  private:
    N mask_;
  };
  */

} // namespace fsu

void SortHeader (const char* name)
{
  std::cout << "\n== " << name << " ==\n";
}

// these functions facilitate the output displays
// first the version for container classes
template <class C>
void Display
            (
	      C                 c,  // container
	      const char*    name,  // container name 
              std::ostream&    os,  // output stream
	      char            ofc   // output format character
            )
{
  os << std::setw(2) << name << ": ";
  if (ofc == '\0')
    for (typename C::Iterator i = c.Begin(); i != c.End(); ++i)
      os << (long)*i;
  else
    for (typename C::Iterator i = c.Begin(); i != c.End(); ++i)
      os << (long)*i << ofc;
  os << '\n';
}

// second an overload for pointers
template <typename T>
void Display
            (
	      T*                A,  // array variable
              size_t         size,  // size of array
	      const char*    name,  // array name 
              std::ostream&    os,  // output stream
	      char            ofc   // output format character
            )
{
  os << std::setw(2) << name << ": ";
  if (ofc == '\0')
    for (T* Aitr = A; Aitr != A + size; ++Aitr)
      os << (long)*Aitr;
  else
    for (T* Aitr = A; Aitr != A + size; ++Aitr)
      os << (long)*Aitr << ofc;
  os << '\n';
}

// this function puts the input data back to the array A for another sort
template <typename S , typename T>
void Restore(S* A, const fsu::Vector< T >& source)
{
  fsu::g_copy (source.Begin(), source.End(), A);
}

int main(int argc, char* argv[])
{
  bool BATCH = 0; 
  char* comfile = 0;
  std::ifstream comstream;
  std::istream* comstreamPtr(&std::cin);
  if (argc < 2)
  {
    comstreamPtr = &std::cin;
    std::cout << " ** No command filename entered - starting in interactive mode\n"
              << "    To execute a file of commands, enter file name on command line\n\n";
    BATCH = 0;
  }
  else if (argc == 2)
  {
    comfile = argv[1];
    comstream.open(comfile);
    if (comstream.fail())
    {
      std::cout << " ** Error: unable to open command file \"" << comfile << "\"\n";
      return (EXIT_FAILURE);
    }
    comstreamPtr = dynamic_cast<std::ifstream*>(&comstream);
    BATCH = 1;
  }
  else 
  {
    std::cout << " ** Error: too many command line arguments\n"
	      << "    for INTERACTIVE mode enter no arguments\n"
	      << "    for BATCH mode enter command file name\n";
    return (EXIT_FAILURE);
  }

  typedef fsu::LessThan <LargeElementType> PredicateType1;
  typedef fsu::GreaterThan <LargeElementType> PredicateType2;

  // PredicateType1                LT; // predicate object
  PredicateType2                GT; // predicate object

  LargeElementType e, s; // entry, sentinal
  fsu::Vector < LargeElementType > inputData; // store user input

  std::cout << "Begin test of numeric sort algorithms < " << e_t << " >\n"
	    << "Enter sentinal (integer): ";
  (*comstreamPtr) >> s;
  if (BATCH) std::cout << s << '\n';

  std::cout << "Enter data (\""
	    << s
	    << "\" to end): ";
  (*comstreamPtr) >> e;
  if (BATCH) std::cout << e << ' ';
  while (e != s)
  {
    inputData.PushBack(e);
    (*comstreamPtr) >> e;
    if (BATCH) std::cout << e << ' ';
  }
  if (BATCH) std::cout << '\n';
  if (BATCH) comstream.close();

  size_t size = inputData.Size();

  // used for counting_sort
  MediumElementType * A4 = new MediumElementType [size];
  MediumElementType * B4 = new MediumElementType [size];

  // these used in bit_sort and byte_sort only:
  TinyElementType   * A1 = new TinyElementType [size];
  SmallElementType  * A2 = new SmallElementType [size];
  LargeElementType  * A8 = new LargeElementType [size];


  std::cout << "data as entered: ";
  Display(inputData,"I",std::cout,ofc);

  // copy to array
  fsu::g_copy (inputData.Begin(), inputData.End(), A4);

  std::cout << "data after copy: ";
  Display(A4,size,"A4",std::cout,ofc);

  // g_heap_sort()
  SortHeader("g_heap_sort()");
  Restore(A4,inputData);
  fsu::g_heap_sort(A4, A4 + size);
  Display(A4,size,"A4",std::cout,ofc);

  // upper bound for data
  size_t k = 1 + *(fsu::g_max_element(inputData.Begin(), inputData.End()));

  // counting_sort()
  SortHeader("counting_sort()");
  Restore(A4,inputData);
  fsu::counting_sort(A4, B4, size, k);
  Display(B4, size, "B4", std::cout, ofc);
  // */

  // upper bound, lower bound, and span for data
  int    ub    = 1 + *(fsu::g_max_element(inputData.Begin(), inputData.End()));
  int    lb    = *(fsu::g_min_element(inputData.Begin(), inputData.End()));
  size_t span  = ub - lb;

  // counting_sort(LowerBound)
  SortHeader("counting_sort(LowerBound)");
  Restore(A4,inputData);
  fsu::LowerBound f(lb);
  fsu::counting_sort(A4, B4, size, span, f);
  Display(B4, size, "B4", std::cout, ofc);
  // */

  // bit_sort tiny type
  SortHeader("bit_sort - type char");
  Restore (A1,inputData);
  std::cout << "array before call:\n";
  Display(A1, size, "A1", std::cout, ofc);
  fsu::bit_sort(A1,size);
  std::cout << "array after call:\n";
  Display(A1, size, "A1", std::cout, ofc);
  // */

  // bit_sort small type
  SortHeader("bit_sort - type short");
  Restore (A2,inputData);
  for (size_t i = 0; i < size; ++i) A2[i] = inputData[i];
  std::cout << "array before call:\n";
  Display(A2, size, "A2", std::cout, ofc);
  fsu::bit_sort(A2,size);
  std::cout << "array after call:\n";
  Display(A2, size, "A2", std::cout, ofc);
  // */

  // bit_sort
  SortHeader("bit_sort - type int");
  Restore(A4,inputData);
  std::cout << "array before call:\n";
  Display(A4, size, "A4", std::cout, ofc);
  fsu::bit_sort(A4,size);
  std::cout << "array after call:\n";
  Display(A4, size, "A4", std::cout, ofc);
  // */

  // bit_sort large type
  SortHeader("bit_sort - type long");
  Restore (A8,inputData);
  std::cout << "array before call:\n";
  Display(A8, size, "A8", std::cout, ofc);
  fsu::bit_sort(A8,size);
  std::cout << "array after call:\n";
  Display(A8, size, "A8", std::cout, ofc);
  // */

  // byte_sort tiny type
  SortHeader("byte_sort - type char");
  Restore (A1,inputData);
  std::cout << "array before call:\n";
  Display(A1, size, "A1", std::cout, ofc);
  fsu::byte_sort(A1,size);
  std::cout << "array after call:\n";
  Display(A1, size, "A1", std::cout, ofc);
  // */

  // byte_sort small type
  SortHeader("byte_sort - type short");
  Restore (A2,inputData);
  std::cout << "array before call:\n";
  Display(A2, size, "A2", std::cout, ofc);
  fsu::byte_sort(A2,size);
  std::cout << "array after call:\n";
  Display(A2, size, "A2", std::cout, ofc);
  // */

  // byte_sort medium type
  SortHeader("byte_sort - type int");
  Restore(A4,inputData);
  std::cout << "array before call:\n";
  Display(A4, size, "A4", std::cout, ofc);
  fsu::byte_sort(A4,size);
  std::cout << "array after call:\n";
  Display(A4, size, "A4", std::cout, ofc);
  // */

  // byte_sort large type
  SortHeader("byte_sort - type long");
  Restore (A8,inputData);
  std::cout << "array before call:\n";
  Display(A8, size, "A8", std::cout, ofc);
  fsu::byte_sort(A8,size);
  std::cout << "array after call:\n";
  Display(A8, size, "A8", std::cout, ofc);
  // */

  // word_sort small type
  SortHeader("word_sort - type short");
  Restore (A2,inputData);
  std::cout << "array before call:\n";
  Display(A2, size, "A2", std::cout, ofc);
  fsu::word_sort(A2,size);
  std::cout << "array after call:\n";
  Display(A2, size, "A2", std::cout, ofc);
  // */

  // word_sort medium type
  SortHeader("word_sort - type int");
  Restore(A4,inputData);
  std::cout << "array before call:\n";
  Display(A4, size, "A4", std::cout, ofc);
  fsu::word_sort(A4,size);
  std::cout << "array after call:\n";
  Display(A4, size, "A4", std::cout, ofc);
  // */

  // word_sort large type
  SortHeader("word_sort - type long");
  Restore (A8,inputData);
  std::cout << "array before call:\n";
  Display(A8, size, "A8", std::cout, ofc);
  fsu::word_sort(A8,size);
  std::cout << "array after call:\n";
  Display(A8, size, "A8", std::cout, ofc);
  // */

  // g_heap_sort(>)
  SortHeader("g_heap_sort(>)");
  Restore(A4,inputData);
  fsu::g_heap_sort(A4, A4 + size, GT);
  Display(A4,size,"A4",std::cout,ofc);

  // counting_sort(Reverse)
  SortHeader("counting_sort(Reverse)");
  Restore(A4,inputData);
  ub = 1 + *(fsu::g_max_element(A4, A4 + size));
  lb = *(fsu::g_min_element(A4, A4 + size));
  k = 1 + ub - lb;
  fsu::Reverse r(ub);
  fsu::counting_sort(A4, B4, size, k, r);
  Display(B4, size, "B4", std::cout, ofc);
  // */

  delete [] A1;
  delete [] A2;
  delete [] A4;
  delete [] A8;
  delete [] B4;

  std::cout << "\nEnd test of numeric sort algorithms < " << e_t << " >\n";

  return 0;
}
