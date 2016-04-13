// students: be sure to supply your own file header in place of the one below!

/*
    gsort_stub.h
    09/17/15
    Chris Lacher

    generic sort algorithms applicable to a range specified by iterators

    includes these generic sort algorithms:

      g_selection_sort
      g_merge_sort
      g_insertion_sort
      g_quick_sort
 
    note that g_heap_sort (all versions) are located in gheap_*.h

    stub version
      - has missing code in a number of places
      - has some code provided to help get started

    Copyright 2015, R. C. Lacher
*/

#include <genalg.h> // Swap, g_copy
#include <gset.h>   // g_set_merge

int cutoff = 0;

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
        if (cmp(*j, *k))
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

  namespace mergesort
  // note: These are supplied complete and ready for use
  {

    // these are the special "merge" functions supporting merge sort
    // note that these are complete and can be used as is

    // for random access iterators and default order
    template < typename RAIterator >
    void Merge(RAIterator beg, RAIterator mid, RAIterator end)
    {
      typename RAIterator::ValueType  B [end - beg];   // temp space for merged copy of A
      g_set_merge(beg, mid, mid, end, B);      // merge the two parts of A to B
      g_copy(B, B+(end-beg), beg);             // copy B back to A[p,r)
    }

    // for random access iterators and order determined by a predicate object
    template < typename RAIterator , class Comparator >
    void Merge(RAIterator beg, RAIterator mid, RAIterator end, Comparator& cmp)
    {
      typename RAIterator::ValueType  B [end - beg];
      g_set_merge(beg, mid, mid, end, B, cmp);
      g_copy(B, B+(end-beg), beg);
    }

    // specialization for pointers and default order
    template < typename T >
    void Merge(T* beg, T* mid, T* end)
    {
      T B [end - beg];                     // temp space for merged copy of A
      g_set_merge(beg, mid, mid, end, B);  // merge the two parts of A to B
      g_copy(B, B+(end-beg), beg);         // copy B back to A[p,r)
    }

    // specialization for pointers and predicate order
    template < typename T , class Comparator >
    void Merge(T* beg, T* mid, T* end, Comparator& cmp)
    {
      T B [end - beg];
      g_set_merge(beg, mid, mid, end, B, cmp);
      g_copy(B, B+(end-beg), beg);
    }

  } // namespace mergesort

  template < class RAIterator >
  void g_merge_sort (RAIterator beg, RAIterator end)
  {
    // recursive
    // calls mergesort::Merge (beg, mid, end);
	size_t size = end - beg;
	if (size > 1)
	{
		RAIterator mid = beg + (size/2);
		g_merge_sort(beg, mid);
		g_merge_sort(mid, end);
		mergesort::Merge(beg, mid, end);
	}
  }

  template < class RAIterator , class Comparator >
  void g_merge_sort (RAIterator beg, RAIterator end, Comparator& cmp)
  {
    // recursive
    // calls mergesort::Merge (beg, mid, end, cmp);
	size_t size = end - beg;
	if (size > 1)
	{
		RAIterator mid = beg + size/2;
		g_merge_sort(beg, mid, cmp);
		g_merge_sort(mid, end, cmp);
		mergesort::Merge(beg, mid, end, cmp);
	}
  }
  
  template < class RAIterator >
  void g_merge_sort_bu (RAIterator beg, RAIterator end)
  {
	  size_t size = end - beg;
	  if (size < 2)
		  return;
	  for (size_t i = 1; i < size; i+=i)
	  {
		  for (size_t j = 0; j < size - i; j += i+i)
		  {
			  if (size < j+i+i)
				  mergesort::Merge(beg+j, beg+j+i, end);
			  else
				  mergesort::Merge(beg+j, beg+j+i, beg+j+i+i);
		  }
	  }
  }

  template < class RAIterator , class Comparator >
  void g_merge_sort_bu (RAIterator beg, RAIterator end, Comparator& cmp)
  {
	  size_t size = end - beg;
	  if (size < 2)
		  return;
	  for (size_t i = 1; i < size; i+=i)
	  {
		  for (size_t j = 0; j < size - i; j += i+i)
		  {
			  if (size < j+i+i)
				  mergesort::Merge(beg+j, beg+j+i, end, cmp);
			  else
				  mergesort::Merge(beg+j, beg+j+i, beg+j+i+i, cmp);
		  }
	  }	  
  }
  
  //optimised g_merge_sort
  //still needs optimization
  
  template < class RAIterator >
  void g_merge_sort_opt (RAIterator beg, RAIterator end)
  {
    // recursive
    // calls mergesort::Merge (beg, mid, end);
	size_t size = end - beg;
	cutoff = 8;
	RAIterator mid = beg + (size/2);
	
	if (size > 1)
	{
		if ((mid - beg) > cutoff)
			g_merge_sort_opt(beg, mid);
		else
			g_insertion_sort(beg, mid);
		if ((end - mid) > cutoff)
			g_merge_sort_opt(mid, end);
		else
			g_insertion_sort(mid, end);
	}
	if (*mid < *(mid-1))
		mergesort::Merge(beg, mid, end);
  }

  template < class RAIterator , class Comparator >
  void g_merge_sort_opt (RAIterator beg, RAIterator end, Comparator& cmp)
  {
    // recursive
    // calls mergesort::Merge (beg, mid, end, cmp);
	size_t size = end - beg;
	cutoff = 8;
	RAIterator mid = beg + (size/2);
	
	if (size > 1)
	{
		if ((mid - beg) > cutoff)
			g_merge_sort_opt(beg, mid, cmp);
		else
			g_insertion_sort(beg, mid, cmp);
		if ((end - mid) > cutoff)
			g_merge_sort_opt(mid, end, cmp);
		else
			g_insertion_sort(mid, end, cmp);
	}
	if (cmp(*(mid-1), *mid))
	{
		//do nothing
	} else {
		mergesort::Merge(beg, mid, end, cmp);
	}
  }
  // Note: specializations of mergesort::Merge are needed to handle pointer
  // cases, but specializations are not needed for g_merge_sort itself

  namespace quicksort
  {

    // namespace supporting quicksort
    // NOTE that inside this namespace iterators define closed ranges [p,r]

    template < class IterType >
    IterType Partition (IterType first, IterType last) // closed range [first,last]
    {
      // this code is complete
      IterType pivot = first;  // initially the pivot position is first position
      for (IterType j = first; j != last; ++j)
      {
        if (!(*last < *j)) // if (*j <= *last)
        {
          Swap(*pivot,*j);
          ++pivot;
        }
      }
      Swap (*pivot,*last);
      return pivot;
    }

    template < class IterType , class P >
    IterType Partition (IterType first, IterType last, P& cmp) // closed range [first,last]
    {
      IterType pivot = first;
      for (IterType j = first; j != last; ++j)
	  {
		if (!(cmp(*last, *j)))
		{
			Swap(*pivot, *j);
			++pivot;
		}
	  }
	  Swap (*pivot, *last);
      return pivot;
    }

  } // namespace

  template < class IterType >
  void g_quick_sort (IterType beg, IterType end)
  {
    // recursive
    // calls quicksort::Partition(beg, end);
	if (end - beg > 1)
	{
		IterType q = quicksort::Partition(beg, end-1);
		g_quick_sort(beg, q);
		g_quick_sort(q+1, end);
	}
  }

  template < class IterType , class Comparator >
  void g_quick_sort (IterType beg, IterType end, Comparator& cmp)
  {
    // recursive
    // calls quicksort::Partition(beg, end, cmp);
	if (end - beg > 1)
	{
		IterType q = quicksort::Partition(beg, end-1, cmp);
		g_quick_sort(beg, q, cmp);
		g_quick_sort(q+1, end, cmp);
	}
  }

  // following are the optimized versions

  template < class IterType >
  void g_quick_sort_opt (IterType beg, IterType end)
  {
    // implementation required
	cutoff = 11;
	if (end - beg > 1)
	{
		if (end - beg > cutoff)
		{
			IterType q = quicksort::Partition(beg, end-1);
			g_quick_sort_opt(beg, q);
			g_quick_sort_opt(++q, end);
		}
		else
		{
			g_insertion_sort(beg, end);
		}
	}
  }

  template < class IterType , class Comparator >
  void g_quick_sort_opt (IterType beg, IterType end, Comparator& cmp)
  {
    // implementation required
	cutoff = 11;
	if (end - beg > 1)
	{
		if (end - beg > cutoff)
		{
			IterType q = quicksort::Partition(beg, end-1, cmp);
			g_quick_sort_opt(beg, q, cmp);
			g_quick_sort_opt(++q, end, cmp);
		}
		else
		{
			g_insertion_sort(beg, end, cmp);
		}
	}
  }

  template < typename T >
  void g_quick_sort_3w (T* beg, T* end)
  {
	  if (end-beg > 1)
	  {
		T* low = beg;
		T* hih = end;
		T v = *beg;
		T* i = beg;
		while (i != hih)
		{
			if (*i < v) Swap(*low++, *i++);
			else if (*i > v) Swap(*i, *--hih);
			else ++i;
		}
		g_quick_sort_3w(beg, low);
		g_quick_sort_3w(hih, end);
	  }
  }

  template < typename T , class Comparator >
  void g_quick_sort_3w (T* beg, T* end, Comparator& cmp)
  {
	  if (end - beg > 1)
	  {
		T* low = beg;
		T* hih = end;
		T v = *beg;
		T* i = beg;
		while (i != hih)
		{
			if (cmp(*i,v)) Swap(*low++, *i++);
			else if (!(cmp(*i, v))) 
			{
				if(*i == v) ++i;
				else Swap(*i, *--hih);
			}
		}
		g_quick_sort_3w(beg, low, cmp);
		g_quick_sort_3w(hih, end, cmp);
	  }
  }
  
  template < typename T >
  void g_quick_sort_3w_opt (T* beg, T* end)
  {
	  int cutoff = 13;
	  
	  if (end-beg > cutoff)
	  {
		T* low = beg;
		T* hih = end;
		T v = *beg;
		T* i = beg;
		while (i != hih)
		{
			if (*i < v) Swap(*low++, *i++);
			else if (*i > v)
			{
				if(*i == v) ++i;
				else Swap(*i, *--hih);
			}
		}
		g_quick_sort_3w_opt(beg, low);
		g_quick_sort_3w_opt(hih, end);
	  }
	  else if(end - beg > 1 && end - beg <= cutoff)
		  g_insertion_sort(beg, end);
  }

  template < typename T , class Comparator >
  void g_quick_sort_3w_opt (T* beg, T* end, Comparator& cmp)
  {
	  int cutoff = 13;
	  
	  if (end-beg > cutoff)
	  {
		T* low = beg;
		T* hih = end;
		T v = *beg;
		T* i = beg;
		while (i != hih)
		{
			if (cmp(*i, v)) Swap(*low++, *i++);
			else if (!cmp(*i,v))
			{
				if(*i == v) ++i;
				else Swap(*i, *--hih);
			}
		}
		g_quick_sort_3w_opt(beg, low, cmp);
		g_quick_sort_3w_opt(hih, end, cmp);
	  }
	  else if(end - beg > 1 && end - beg <= cutoff)
		  g_insertion_sort(beg, end, cmp);
  }

  template < class IterType >
  void g_quick_sort_3w (IterType beg, IterType end)
  {
    // implementation required
	if(end - beg > 1) 
	{
		IterType low = beg;
		IterType hih = end;
		typename IterType::ValueType v = *beg;
		IterType i = beg;
		while (i != hih)
		{
			if (*i < v) Swap(*low++, *i++);
			else if (*i > v) Swap(*i, *--hih);
			else ++i;
		}
		g_quick_sort_3w(beg, low);
		g_quick_sort_3w(hih, end);
	}
  }

  template < class IterType , class Comparator >
  void g_quick_sort_3w (IterType beg, IterType end, Comparator& cmp)
  {
    // implementation required
	if (end - beg > 1)
	{
		IterType low = beg;
		IterType hih = end;
		typename IterType::ValueType v = *beg;
		IterType i = beg;
		while (i != hih)
		{
			if (cmp(*i, v)) Swap(*low++, *i++);
			else if (!(cmp(*i, v))) {
				if (*i == v)
					++i;
				else
					Swap(*i, *--hih);
			}
		}
		g_quick_sort_3w(beg, low, cmp);
		g_quick_sort_3w(hih, end, cmp);
	}
  }

    template < class IterType >
  void g_quick_sort_3w_opt (IterType beg, IterType end)
  {
    // implementation required
	cutoff = 13;
	if(end - beg > 1)
	{
		if(end - beg > cutoff)
		{
			IterType low = beg;
			IterType hih = end;
			typename IterType::ValueType v = *beg;
			IterType i = beg;
			while (i != hih)
			{
				if (*i < v) Swap(*low++, *i++);
				else if (*i > v) Swap(*i, *--hih);
				else ++i;
			}
			g_quick_sort_3w_opt(beg, low);
			g_quick_sort_3w_opt(hih, end);
		}
		else if(end - beg > 1 && end - beg <= cutoff)
		{
			g_insertion_sort(beg, end);
		}
	}
  }

  template < class IterType , class Comparator >
  void g_quick_sort_3w_opt (IterType beg, IterType end, Comparator& cmp)
  {
    // implementation required
	cutoff = 13;
	if(end - beg > 1)
	{
		if(end - beg > cutoff)
		{
			IterType low = beg;
			IterType hih = end;
			typename IterType::ValueType v = *beg;
			IterType i = beg;
			while (i != hih)
			{
				if (cmp(*i, v)) Swap(*low++, *i++);
				else if (!(cmp(*i, v))) 
				{
					if(*i == v)
						++i;
					else
						Swap(*i, *--hih);
				}
			}
			g_quick_sort_3w_opt(beg, low, cmp);
			g_quick_sort_3w_opt(hih, end, cmp);
		}
		else if(end - beg > 1 && end - beg <= cutoff)
		{
			g_insertion_sort(beg, end, cmp);
		}
	}
  }
  // note: missing are g_merge_sort_opt, g_merge_sort_bu, g_quick_sort_3w_opt

} // namespace fsu
