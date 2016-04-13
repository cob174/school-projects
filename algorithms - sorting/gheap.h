/*
    gheap.h
    04/01/14
    Chris Lacher

    generic heap algorithms

    advanced set includes push_heap, pop_heap, heap_sort, plus
    heap_repair and build_heap.

    In addition, heap_sort has improved efficiency by using build_heap for the
    first stage, which has runtime Theta(n), whereas the loop of calls to
    push_heap, used in the basic implementation, has runtime Theta(n log n). Of
    course, heap_sort itself still has runtime Theta(n log n) due to the second
    loop, so this is a marginal improvement.

    These algorithms use three types
    but only two template parameters <I,P>

      I: a random access iterator class
      P: a predicate class for T
      I::ValueType for underlying container on which I iterates

    A random access iterator I is one for which the bracket operator 
      T& operator [] (size_t );
    is defined and for which "pointer" arithmetic is defined. 
    Examples include: ordinary arrays of T; TVector<T>; and TDeque<T>

    A predicate class is one for which the function evaluation operator 
      bool operator () (const T&, const T&);
    is defined. An object pred of class P is used to determine order
    in T: "a < b" iff "true = pred (a,b)"

    ValueType is not explicitly mentioned in the template (or parameter)
    list for g_heap_sort, but it is needed for the call to g_XC.

    g_build_heap has runtime ~2n [see Cormen pp. 158-9]

    Copyright 2014, R. C. Lacher
*/

#ifndef _GHEAP_H
#define _GHEAP_H

#include <cstdlib>   // size_t

namespace fsu
{

  template <class I, class P>
  void g_push_heap (I beg, I end, P& pred);
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  //       if the range [beg,end-1) is valid (i.e., [beg,end) is non-void)
  //         then the range [beg,end-1) is a POT
  // Post: the range [beg,end)   has been permuted
  //       the range [beg,end)   is a POT

  template <class I>
  void g_push_heap (I beg, I end);
  // the default order version

  template <class I, class P>
  void g_pop_heap (I beg, I end, P& pred);
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  //       the range [beg,end-1) is valid (i.e., [beg,end) is non-void)
  //       the range [beg,end)   is a POT 
  // Post: the largest element beg[0] is removed from the range [beg,end-1)
  //       the input range [beg+1,end) has been permuted to the output range [beg,end-1)
  //       the range [beg,end-1) is a POT
  //       the old largest element (i.e., beg[0]) is copied to 
  //         the end of the old range (i.e., to beg[end - beg - 1])

  template <class I>
  void g_pop_heap (I beg, I end);
  // the default order version

  template <class I, class P>
  void g_heap_sort (I beg, I end, P& pred);
  // Implements heapsort for the iterators
  // Heapsort has the following attributes:
  //   * fast:     runtime O(size*log(size))
  //   * in-place: the sort happens in the space implied by the iterators
  //   * NOT stable -- the original order of objects a, b may change when
  //                   a == b
  // pre:  I is a random access iterator class (operator [] and "pointer" arithmetic)
  //       T is the ValueType of I
  //       P is a predicate class for type T
  // post: the range of values specified by the iterators is ordered by pred,
  //       i.e., true = pred(a,b)  <==>  a comes before b in the container

  template <class I>
  void g_heap_sort (I beg, I end);
  // the default order version

  template <typename I, class P>
  void g_build_heap (I beg, I end, P& pred);
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  // Post: the range [beg,end)   is a POT

  template <class I>
  void g_build_heap (I beg, I end);
  // the default order version

  template <typename I, class P>
  void g_heap_repair(I beg, I loc, I end, P& p);
  // In:  i < end - beg
  //      subtrees rooted at Left(i) and Right(i) are heaps
  // Out: subtree rooted at i is a heap

  template <typename I>
  void g_heap_repair(I beg, I loc, I end);
  // the default order version

  template <typename T>
  void g_XC (T& t1, T& t2);

} // namespace fsu

namespace alt
// uses g_push_heap loop instead of g_build_heap
{

  template <class I, class P>
  void g_heap_sort (I beg, I end, P& pred);

  template <class I>
  void g_heap_sort (I beg, I end);

} // namespace alt

namespace fsu
{

  //-------------------------------------
  // g_build_heap (iter, iter, pred)
  //-------------------------------------

  template <class I, class P>
  void g_heap_repair(I beg, I loc, I end, P& p)
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  //       both left and right subtrees of loc are POT
  // Post: loc is POT
  {
    if (end - loc < 1)
      return;

    size_t n,i,l,r,largest;
    n = end - beg;
    i = loc - beg;
    bool finished = 0;
    do
    {
      // l = 2*i + 1; r = l + 1;
      l = (i << 1) | (size_t)0x01; r = l + 1;
      largest = ((l < n && p(beg[i], beg[l])) ? l : i);
      if (r < n && p(beg[largest], beg[r]))
        largest = r;

      // test order property at i; if bad, swap and repeat
      if (largest != i)
      {
        ::fsu::g_XC(beg[i],beg[largest]);
        i = largest;
      }
      else finished = 1;
    }
    while (!finished);
  } // g_heap_repair (beg,loc,end,p) - compact version */

  template <class I>
  void g_heap_repair(I beg, I loc, I end)
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  //       both left and right subtrees of loc are POT
  // Post: loc is POT
  {
    if (end - loc < 1)
      return;
    size_t n = end - beg;
    size_t i = loc-beg, l, r;
    bool finished = 0;
    do
    {
      // l = 2*i + 1; r = l + 1;  // left and right child nodes
      l = (i << 1) | 0x01; r = l + 1;
      if (r < n)               // both child nodes exist
      {
        if (beg[l] < beg[r])
        // ==> follow right subtree
        {
          if (beg[i] < beg[r])
          {
            g_XC(beg[r], beg[i]);
            i = r;
          }
          else
          {
            finished = 1;
          }
        } // end if
        else // (!p(beg[l],beg[r]))
        // ==> follow left subtree
        {
          if (beg[i] < beg[l])
          {
            g_XC(beg[l], beg[i]);
            i = l;
          }
          else
          {
            finished = 1;
          }
        }  // end else
      } // end if
      else if (l < n)
      // only the left child node exists
      {
        if (beg[i] < beg[l])
        {
          g_XC(beg[l], beg[i]);
        }
        finished = 1;          // no grandchild nodes exist
      } // end else if
      else                     // no child nodes exist
      {
        finished = 1;
      } // end else
    }
    while (!finished);
  } // g_heap_repair (beg,loc,end) - expansive version */

  template <typename I, class P>
  void g_build_heap (I beg, I end, P& pred)
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  // Post: the range [beg,end)   is a POT
  {
    size_t size = end - beg;
    if  (size < 2) return;
    for (size_t i = size/2; i > 0; --i)
    {
      g_heap_repair(beg,beg+(i-1),end,pred);
    }
  }

  template <class I>
  void g_build_heap (I beg, I end)
  // default order version
  {
    size_t size = end - beg;
    if  (size < 2) return;
    for (size_t i = size/2; i > 0; --i)
    {
      fsu::g_heap_repair(beg,beg+(i-1),end);
    }
  }

  //-------------------------------------
  // g_heap_sort (iter, iter, pred)
  //-------------------------------------

  template <class I, class P>
  void g_heap_sort (I beg, I end, P& pred)
  // Implements heapsort for the iterators
  // Heapsort has the following attributes:
  //   * fast:     runtime O(size*log(size))
  //   * in-place: the sort happens in the space implied by the iterators
  //   * NOT stable -- the original order of objects a, b may change when
  //                   a == b
  // pre:  I is a random access iterator class (operator [] and "pointer" arithmetic)
  //       T is the ValueType of I
  //       P is a predicate class for type T
  // post: the range of values specified by the iterators is ordered by pred,
  //       i.e., true = pred(a,b)  <==>  a comes before b in the container
  {
    if (end - beg <= 1)
      return;
    size_t size = end - beg;
    size_t i; 
    g_build_heap(beg, end, pred);
    for (i = size; i > 1; --i)
    {
      g_pop_heap(beg, beg + i, pred);
    }
  } // end g_heap_sort()

  template <class I>
  void g_heap_sort (I beg, I end)
  // the default order version
  {
    if (end - beg <= 1)
      return;
    size_t size = end - beg;
    size_t i; 
    g_build_heap(beg, end);
    for (i = size; i > 1; --i)
    {
      g_pop_heap(beg, beg + i);
    }
  } // end g_heap_sort()

  //---------------------------------
  // generic push/pop heap algorithms 
  //---------------------------------

  template <class I, class P>
  void g_push_heap (I beg, I end, P& pred)
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  //       if the range [beg,end-1) is valid (i.e., [beg,end) is non-void)
  //         then the range [beg,end-1) is a POT
  // Post: the range [beg,end)   has been permuted
  //       the range [beg,end)   is a POT
  {
    size_t n = end - beg;
    size_t i = n - 1, p;
    bool finished = 0;
    do
    {
      if (i > 0)
      {
        // p = (i - 1) / 2;  // parent node
        p = (i - 1) >> 1;
        if (pred(beg[p],beg[i]))
        {
          g_XC(beg[i], beg[p]);
          i = p;
        }
        else
        {
          finished = 1;
        }
      }
      else
      {
        finished = 1;
      }
    }
    while (!finished);
  } // end g_push_heap()

  template <class I>
  void g_push_heap (I beg, I end)
  // the default order version
  {
    size_t n = end - beg;
    size_t i = n - 1, p;
    bool finished = 0;
    do
    {
      if (i > 0)
      {
        // p = (i - 1) / 2;  // parent node
        p = (i - 1) >> 1;
        if (beg[p] < beg[i])
        {
          g_XC(beg[i], beg[p]);
          i = p;
        }
        else
        {
          finished = 1;
        }
      }
      else
      {
        finished = 1;
      }
    }
    while (!finished);
  } // end g_push_heap()

  template <class I, class P>
  void g_pop_heap (I beg, I end, P& pred)
  // Defn: a < b means true = pred(a,b)
  // Defn: POT means parent >= children
  // Pre:  the range [beg,end)   is valid
  //       the range [beg,end-1) is valid (i.e., [beg,end) is non-void)
  //       the range [beg,end)   is a POT 
  // Post: the largest element beg[0] is removed from the range [beg,end-1)
  //       the input range [beg+1,end) has been permuted to the output range [beg,end-1)
  //       the range [beg,end-1) is a POT
  //       the old largest element (i.e., beg[0]) is copied to 
  //         the end of the old range (i.e., to beg[end - beg - 1])
  {
    if (end - beg < 2)
      return;
    g_XC(*beg,*(end - 1));
    g_heap_repair(beg,beg,end - 1,pred);
  } // end g_pop_heap()

  template <class I>
  void g_pop_heap (I beg, I end)
  // the default order version
  {
    if (end - beg < 2)
      return;
    g_XC(*beg,*(end - 1));
    g_heap_repair(beg,beg,end - 1);
  } // end g_pop_heap()

  template <typename T>
  void g_XC (T& t1, T& t2)
  {
    T temp(t1);
    t1 = t2;
    t2 = temp;
  }
  
} // namespace fsu

namespace alt
{

  template <class I, class P>
  void g_heap_sort (I beg, I end, P& pred)
  // Implements heapsort for the iterators
  // Heapsort has the following attributes:
  //   * fast:     runtime O(size*log(size))
  //   * in-place: the sort happens in the space implied by the iterators
  //   * NOT stable -- the original order of objects a, b may change when
  //                   a == b
  // pre:  I is a random access iterator class (operator [] and "pointer" arithmetic)
  //       T is the ValueType of I
  //       P is a predicate class for type T
  // post: the range of values specified by the iterators is ordered by pred,
  //       i.e., true = pred(a,b)  <==>  a comes before b in the container
  {
    if (end - beg <= 1)
      return;
    size_t size = end - beg;
    size_t i; 
    // push elements onto heap one at a time
    for (i = 1; i < size; ++i)
    {
      fsu::g_push_heap(beg, beg + (i + 1), pred);
    }
    
    // keep popping largest remaining element to end of remaining array
    for (i = size; i > 1; --i)
    {
      fsu::g_pop_heap(beg, beg + i, pred);
    }
  } // end g_heap_sort()

  template <class I>
  void g_heap_sort (I beg, I end)
  // the default order version
  {
    if (end - beg <= 1)
      return;
    size_t size = end - beg;
    size_t i; 
    for (i = 1; i < size; ++i)
    {
      fsu::g_push_heap(beg, beg + (i + 1));
    }
    for (i = size; i > 1; --i)
    {
      fsu::g_pop_heap(beg, beg + i);
    }
  } // end g_heap_sort()

} // namespace alt

#endif
