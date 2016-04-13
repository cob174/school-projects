/*
    gsort.h
    09/13/13
    Chris Lacher

    generic sort algorithms applicable to a range specified by iterators

    includes these generic sort algorithms:

      g_selection_sort
      # g_merge_sort
      g_insertion_sort
      # g_quick_sort
 
    note that g_heap_sort is located in gheap.h

    Copyright 2013, R. C. Lacher
*/

#include <genalg.h> // Swap, g_copy

const int recursion_cutoff_threshold = 10;

namespace fsu
{

  template < class ForwardIterator >
  void g_selection_sort (ForwardIterator beg, ForwardIterator end)
  {
    ForwardIterator i, j, k;
    for (i = beg; i != end; ++i)
    {
      k = i;
      for (j = i; j != end; ++j)
        if (*j < *k)
          k = j;
      Swap (*i, *k);
    }
  }

  template < class ForwardIterator, class Comparator >
  void g_selection_sort (ForwardIterator beg, ForwardIterator end, Comparator& cmp)
  {
    ForwardIterator i, j, k;
    for (i = beg; i != end; ++i)
    {
      k = i;
      for (j = i; j != end; ++j)
        if (cmp(*j,*k))
          k = j;
      Swap (*i, *k);
    }
  }

  template < class BidirectionalIterator >
  void g_insertion_sort (BidirectionalIterator beg, BidirectionalIterator end)
  {
    BidirectionalIterator i, j, k;
    typename BidirectionalIterator::ValueType t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && t < *k; --j, --k)
        *j = *k;
      *j = t;
    }
  }

  template < class BidirectionalIterator, class Comparator >
  void g_insertion_sort (BidirectionalIterator beg, BidirectionalIterator end, Comparator& cmp)
  {
    BidirectionalIterator i, j, k;
    typename BidirectionalIterator::ValueType t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && cmp(t,*k); --j, --k)
        *j = *k;
      *j = t;
    }
  }

  // specialization for pointers
  template < typename T >
  void g_insertion_sort (T* beg, T* end)
  {
    // fsu::Debug ("g_insertion_sort");
    T *i, *j, *k;
    T t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && t < *k; --j, --k)
        *j = *k;
      *j = t;
    }
  }

  // specialization for pointers
  template < typename T , class Comparator >
  void g_insertion_sort (T* beg, T* end, Comparator& cmp)
  {
    T *i, *j, *k;
    T t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && cmp(t,*k); --j, --k)
        *j = *k;
      *j = t;
    }
  }

} // namespace 
