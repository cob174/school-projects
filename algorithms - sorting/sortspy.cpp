/*
   sortspy.cpp
   09/26/15
   Chris Lacher

   takes two arguments: input and output file names

   processes as follows: 

     reads data from input file
     for each sort:
       sorts data
       collects CPU time between start and end of sort
       collects comp_count data for comparison sorts
       writes timing data to output file (and screen)
       writes comp_count data to output file (and screen)

   Timing is done with an array of data to minimize container overhead
   and pointer dereference time. The sizes of the data sets should be not much
   bigger than uint_max = 2^32 - 1, because the random number generator supplies
   unsigned int data. Using that kind of data for larger data sets will produce
   a lot of repeated values [the pigeonhole principle...].

   Comp_count data is collected using a "spy" less-than predicate object that
   counts the number of times it is called. When "spy" is used, the timing data
   is useful for relative comparison among the comparison sort algorithms, but
   it is inflated by the cost of spying. Better time estimates can be obtained
   using pure predicates.

   Types are important for other reasons as well. The random number generator
   uses the extra bits in unsigned long as a kind of larger universe to stir up
   unsigned int to make the latter appear random. The element type of the 
   containing array we are sorting should also be unsigned int, otherwise
   putting bit_sort through unnecessary work. (Bit sort should know the number
   of bits in the data it is sorting.)

*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <climits>
#include <vector.h>
#include <genalg.h>
// #include <gheap.h>       // used in cop4530
// #include <gheap_basic.h> // used in cop4530
#include <gheap_advanced.h>
#include <gheap_cormen.h>
#include <gsort.h>
#include <nsort.h>
#include <timer.cpp>
#include <list.h>
#include <insert.h>
#include <compare_spy.h>
#include <compare.h>
// #include <gran.h>        // g_random_shuffle

const int c1 = 20;
// const int c1 = 24;       // space needed for randomized cases
const int c2 = 10, c3 = 15, c4 = 6, c5 = 15, c6 = 15; // column widths

typedef uint32_t ElementType;

template < class I , class P >
size_t CheckOrder (I beg, I end, P& p, bool verbose = 0)
{
  // if range has 0 or 1 element, order is ok
  if (beg == end) return 0;
  I i = beg;
  I j = i++;
  if (i == end) return 0;

  size_t position = 0;
  size_t count = 0;
  while (i != end)
  {
    if (p(*i,*j))
    {
      if (verbose)
      {
        std::cout << " ** order discrepancy at c[" << position << "] = " << *i << '\n';
      }
      ++count;
    }
    ++ position;
    j = i++;
  }
  return count;
}

typedef uint32_t NumberType;

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << " ** command line arguments:\n"
	      << "     1: input filename (required)\n"
	      << "     2: output filename (required)\n"
	      << "     3: \"fast\" (optional) - omits Theta(n^2) sorts\n"
	      << " ** try again\n";
    return 0;
  }

  char* infile      = argv[1];
  char* outfile     = argv[2];
  fsu::LessThanSpy < ElementType > lts;
  fsu::LessThan    < ElementType > lt;
  // fsu::GreaterThanSpy < ElementType > lts;
  // fsu::GreaterThan    < ElementType > lt;
  std::ifstream in1(infile);
  if (in1.fail())
  {
    std::cout << " ** cannot open file " << infile << " for read\n"
	      << " ** try again\n";
    return 0;
  }

  std::ofstream out1(outfile);
  if (out1.fail())
  {
    std::cout << " ** cannot open file " << outfile << " for write\n"
	      << " ** try again\n";
    return 0;
  }

  fsu::Vector < ElementType >   dataStore;
  fsu::List   < ElementType >   dataList;
  fsu::PushBackIterator < fsu::List < ElementType > > listBackPusher(dataList);
  size_t error_count = 0;

  ElementType item;
  while (in1 >> item)
    dataStore.PushBack(item);
  in1.clear();
  in1.close();
  if (dataStore.Size() > UINT_MAX)
  {
    std::cout << " ** WARNING: too many items, keep count <= " << UINT_MAX << '\n';
  }
  size_t size = dataStore.Size();
  // size_t sizesize = size * (size + 1) >> 1;
  // double sizelogsize = size * log2(size);
  // size_t sizelogsize = size * log2(size);

  // this is where we will run the comparison sorts:
  ElementType * data = new ElementType [size];

  // stopwatch
  fsu::Instant instant, instant1, instant2;
  fsu::Timer timer;

  std::cout << "\n Input file name: " << infile << '\n'
	    << "            size: " << size << '\n'
            << '\n';
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) std::cout << '='; std::cout << '\n';
  std::cout << "Comparison Sorts\n";
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) std::cout << '='; std::cout << '\n';
  std::cout << std::left << std::setw(c1) << " algorithm"
            << std::right << std::setw(c2) << "errors"
            << std::setw(c3) << "comp_count" 
            // << std::setw(c4) << "usec(spy)"
	    << std::setw(c4+c5) << "usec"
            << std::setw(c6) << "sec"
            << '\n'
            << std::left << std::setw(c1) << " ---------------"
            << std::right << std::setw(c2) << "------"
            << std::setw(c3) << "----------" 
            // << std::setw(c4) << "----------"
            << std::setw(c4+c5) << "----------"
            << std::setw(c6) << "----------"
            << '\n';
  out1      << "\n -"
	    << infile
	    << "----------------------------------------------\n\n"
	    << " Input file name: " << infile << '\n'
	    << "            size: " << size << '\n'
            << '\n';
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) out1 << '='; out1 << '\n';
  out1      << "Comparison Sorts\n";
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) out1 << '='; out1 << '\n';
  out1      << std::left << std::setw(c1) << " algorithm"
            << std::right << std::setw(c2) << "errors"
            << std::setw(c3) << "comp_count" 
            // << std::setw(c4) << "usec(spy)"
	    << std::setw(c4+c5) << "usec"
            << std::setw(c6) << "sec"
            << '\n'
            << std::left << std::setw(c1) << " ---------------"
            << std::right << std::setw(c2) << "------"
            << std::setw(c3) << "----------" 
            // << std::setw(c4) << "----------"
            << std::setw(c4+c5) << "----------"
            << std::setw(c6) << "----------"
            << '\n';

  std::cout << std::fixed << std::setprecision(6) << std::showpoint;
  out1      << std::fixed << std::setprecision(6) << std::showpoint;

  // selection sort
  if (argc < 4)
  {
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_selection_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_selection_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  //  error_count = 0;
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_selection_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_selection_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  }
  // */

  // insertion sort
  if (argc < 4)
  {
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_insertion_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_insertion_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_insertion_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_insertion_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  }
  // */

  // quick sort
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_quick_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_quick_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  /* // quick sort (randomized)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // quick sort (opt)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_quick_sort_opt(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_quick_sort_opt(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_opt"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_opt"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  /* // quick sort (opt) (randomized)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort_opt(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort_opt(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_opt_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_opt_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // quick sort (3w)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_quick_sort_3w(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_quick_sort_3w(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_3w    "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_3w    "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  /* // quick sort (3w) (randomized)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort_3w(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort_3w(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_3w_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_3w_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // quick sort (3w_opt)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_quick_sort_3w_opt(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_quick_sort_3w_opt(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_3w_opt"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_3w_opt"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  /* // quick sort (3w_opt) (randomized)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort_3w_opt(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_random_shuffle(data, data + dataStore.Size());
  fsu::g_quick_sort_3w_opt(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_quick_sort_3w_opt_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_quick_sort_3w_opt_ran"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // merge sort (rec)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_merge_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_merge_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_merge_sort       "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_merge_sort       "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // merge sort (opt)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_merge_sort_opt(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_merge_sort_opt(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_merge_sort_opt   "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_merge_sort_opt   "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // merge sort (bu)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_merge_sort_bu(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_merge_sort_bu(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " g_merge_sort_bu    "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " g_merge_sort_bu    "
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // list sort (in-place merge sort)
  fsu::g_copy (dataStore.Begin(), dataStore.End(), listBackPusher);

  timer.SplitReset();
  dataList.Sort(lt);
  instant2 = timer.SplitTime();

  dataList.CheckLinks();
  error_count = 0;
  error_count = CheckOrder(dataList.Begin(),dataList.End(),lt);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), dataList.Begin());
  dataList.CheckLinks();

  lts.Reset();
  timer.SplitReset();
  dataList.Sort(lts);
  instant1 = timer.SplitTime();

  dataList.CheckLinks();
  error_count += CheckOrder(dataList.Begin(),dataList.End(),lt);
  std::cout << std::left << std::setw(c1) << " List::Sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " List::Sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // alt::heap sort
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  alt::g_heap_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  alt::g_heap_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " alt::g_heap_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " alt::g_heap_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // fsu::heap sort
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  fsu::g_heap_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  fsu::g_heap_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " fsu::g_heap_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " fsu::g_heap_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // cormen::heap sort
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  lts.Reset();
  timer.SplitReset();
  cormen::g_heap_sort(data , data + dataStore.Size(), lts);
  instant1 = timer.SplitTime();
  error_count = 0;
  error_count = CheckOrder(data,data+size,lt,0);
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  cormen::g_heap_sort(data , data + dataStore.Size(), lt);
  instant2 = timer.SplitTime();
  error_count += CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " cormen::g_heap_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " cormen::g_heap_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << lts.Count()
            // << std::setw(c4) << instant1.Get_useconds()
            << std::setw(c4+c5) << instant2.Get_useconds()
            << std::setw(c6) << instant2.Get_seconds()
            << '\n';
  // */

  // non-comparison sorts
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) std::cout << '='; std::cout << '\n';
  std::cout << "Numerical Sorts\n";
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) std::cout << '='; std::cout << '\n';
  std::cout << std::left << std::setw(c1) << "algorithm          "
            << std::right << std::setw(c2) << "errors"
            << std::setw(c3) << "outer loop" 
            << std::setw(c4) << "size"
	    << std::setw(c5) << "usec"
            << std::setw(c6) << "sec"
            << '\n'
            << std::left << std::setw(c1) << "---------          "
            << std::right << std::setw(c2) << "------"
            << std::setw(c3) << "----------" 
            << std::setw(c4) << "----"
	    << std::setw(c5) << "----"
            << std::setw(c6) << "---"
            << '\n';
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) out1 << '=';  out1 << '\n';
  out1      << "Numerical Sorts\n";
  for (size_t i = 0; i < c1+c2+c3+c4+c5+c6; ++i) out1 << '=';  out1 << '\n';
  out1      << std::left << std::setw(c1) << "algorithm"
            << std::right << std::setw(c2) << "errors"
            << std::setw(c3) << "outer loop" 
            << std::setw(c4) << "size"
	    << std::setw(c5) << "usec"
            << std::setw(c6) << "sec"
            << '\n'
            << std::left << std::setw(c1) << "---------          "
            << std::right << std::setw(c2) << "------"
            << std::setw(c3) << "----------" 
            << std::setw(c4) << "----"
	    << std::setw(c5) << "----"
            << std::setw(c6) << "---"
            << '\n';
  // */

  // counting sort
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  NumberType max = *(fsu::g_max_element(dataStore.Begin(), dataStore.End()));
  if (max <= (UINT_MAX >> 1))
  {
    NumberType * B = new NumberType [dataStore.Size()];
    fsu::counting_sort (data, B, dataStore.Size(), 1+max);
    fsu::g_copy (B, B+dataStore.Size(), data);
    delete [] B;
    instant = timer.SplitTime();
    error_count = CheckOrder(data,data+size,lt,0);
    std::cout << std::left  << std::setw(c1) << " counting_sort"
              << std::right << std::setw(c2) << error_count
              << std::setw(c3) << " -"
              << std::setw(c4) << " -"
              << std::setw(c5) << instant.Get_useconds()
              << std::setw(c6) << instant.Get_seconds()
              << '\n';
    out1      << std::left << std::setw(c1) << " counting_sort"
              << std::right << std::setw(c2) << error_count
              << std::setw(c3) << " -"
              << std::setw(c4) << " -"
              << std::setw(c5) << instant.Get_useconds()
              << std::setw(c6) << instant.Get_seconds()
              << '\n';
  }
  else
  {
    std::cout << std::left  << std::setw(c1) << " counting_sort"
              << std::right << std::setw(c2+c3+c4) << "/* max element size ="
              << ' ' << max << " [too large] */"
              << '\n';
    out1      << std::left  << std::setw(c1) << "counting_sort"
              << std::right << std::setw(c2+c3+c4+c5+c6) << "/* numbers too large */"
              << '\n';
  }
  // */

  // these are used testing bit_sort, byte_sort, and word_sort
  typedef uint8_t      TinyElementType;    // 1
  typedef uint16_t     SmallElementType;   // 2
  typedef uint32_t     MediumElementType;  // 4
  typedef uint64_t     LargeElementType;   // 8

  // bit sort 8
  TinyElementType * data8 = new TinyElementType [size];
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data8); // obvious overflow possibilities
  timer.SplitReset();
  size_t loop_count = fsu::bit_sort (data8, size);
  instant = timer.SplitTime();
  error_count = CheckOrder(data8,data8+size,lt,0);
  std::cout << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // bit sort 16
  SmallElementType * data16 = new SmallElementType [size];
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data16); // obvious overflow possibilities
  timer.SplitReset();
  loop_count = fsu::bit_sort (data16, size);
  instant = timer.SplitTime();
  error_count = CheckOrder(data16,data16+size,lt,0);
  std::cout << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // bit sort 32
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  loop_count = fsu::bit_sort (data, dataStore.Size());
  instant = timer.SplitTime();
  error_count = CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // bit sort 64
  LargeElementType * data64 = new LargeElementType [size];
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data64);
  timer.SplitReset();
  loop_count = fsu::bit_sort (data64, size);
  instant = timer.SplitTime();
  error_count = CheckOrder(data64,data64+size,lt,0);
  std::cout << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " bit_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bits"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // byte sort 8
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data8);
  timer.SplitReset();
  loop_count = fsu::byte_sort (data8, size);
  instant = timer.SplitTime();
  error_count = CheckOrder(data8,data8+size,lt,0);
  std::cout << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // byte sort 16
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data16);
  timer.SplitReset();
  loop_count = fsu::byte_sort (data16, size);
  instant = timer.SplitTime();
  error_count = CheckOrder(data16,data16+size,lt,0);
  std::cout << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // byte sort 32
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  loop_count = fsu::byte_sort (data, dataStore.Size());
  instant = timer.SplitTime();
  error_count = CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // byte sort 64 
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data64);
  timer.SplitReset();
  loop_count = fsu::byte_sort (data64, dataStore.Size());
  instant = timer.SplitTime();
  error_count = CheckOrder(data64,data64+size,lt,0);
  std::cout << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " byte_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "bytes"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // word sort 16
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data16);
  timer.SplitReset();
  loop_count = fsu::word_sort (data16, dataStore.Size());
  instant = timer.SplitTime();
  error_count = CheckOrder(data16,data16+size,lt,0);
  std::cout << std::left << std::setw(c1) << " word_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "words"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " word_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "words"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // word sort 32
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data);
  timer.SplitReset();
  loop_count = fsu::word_sort (data, dataStore.Size());
  instant = timer.SplitTime();
  error_count = CheckOrder(data,data+size,lt,0);
  std::cout << std::left << std::setw(c1) << " word_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "words"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " word_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "words"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  // word sort 64
  fsu::g_copy (dataStore.Begin(), dataStore.End(), data64);
  timer.SplitReset();
  loop_count = fsu::word_sort (data64, dataStore.Size());
  instant = timer.SplitTime();
  error_count = CheckOrder(data64,data64+size,lt,0);
  std::cout << std::left << std::setw(c1) << " word_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "words"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << std::left << std::setw(c1) << " word_sort"
            << std::right << std::setw(c2) << error_count
            << std::setw(c3) << "words"
            << std::setw(c4) << loop_count
            << std::setw(c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  // */

  delete [] data;
  delete [] data8;
  delete [] data16;
  delete [] data64;
  instant = timer.EventTime();
  std::cout << '\n'
            << std::left << std::setw(c1) << " Total Time:"
            << std::right << std::setw(c2+c3+c4+c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';
  out1      << '\n'
            << std::left << std::setw(c1) << " Total Time:"
            << std::right << std::setw(c2+c3+c4+c5) << instant.Get_useconds()
            << std::setw(c6) << instant.Get_seconds()
            << '\n';

  std::cout << '\n' << " Process complete\n\n";
  return 0;
}

