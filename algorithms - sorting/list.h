/*
    list.h
    01/01/14
    Chris Lacher

    Definition the fsu::List<T> API

    Fall 2013:   made predicate objects non-const to use "spyware".

    Copyright 2014, R.C. Lacher
*/

#ifndef _LIST_H
#define _LIST_H

#include <iostream>    // class ostream and objects cerr, cout
#include <cstdlib>     // EXIT_FAILURE, size_t

namespace fsu
{

  template < typename T >
  class List;

  template < typename T >
  class ConstListIterator;

  template < typename T >
  class ListIterator;

  //----------------------------------
  //     List<T>
  //----------------------------------

  template < typename T >
  class List
  {

  public:
    // terminology support: 
    // some scope List<T>:: type definitions to facilitate clients
    typedef T                                      ValueType;
    typedef ListIterator < T >                     Iterator;
    typedef ConstListIterator < T >                ConstIterator;

    // constructors and assignment
                   List       ();              // default constructor
    virtual        ~List      ();              // destructor
                   List       (const List& );  // copy constructor
    List&          operator = (const List& );  // assignment
    virtual List * Clone      () const;        // returns ptr to deep copy of this list [13]

    // modifying List structure - mutators
    bool      PushFront  (const T& t);   // Insert t at front of list [3] [4]
    bool      PushBack   (const T& t);   // Insert t at back of list
    Iterator  Insert     (Iterator i, const T& t);  // Insert t at i  [5]
    ConstIterator  Insert     (ConstIterator i, const T& t);  // ConstIterator version
    Iterator  Insert     (const T& t);   // Insert t                  [6]
    List&     operator+= (const List& list); // append list

    bool      PopFront  ();              // Remove the Tval at front
    bool      PopBack   ();              // Remove the Tval at back 
    Iterator  Remove    (Iterator i);    // Remove item at I          [7]
    ConstIterator  Remove    (ConstIterator i);    // ConstIterator version
    size_t    Remove    (const T& t);    // Remove all copies of t    [8]
    void      Clear     ();              // Empty the list, deleting all elements
    // NOTE: pop and remove operations may make some iterators illegitimate! 

    // macroscopic (whole list) mutators
    void      Sort      ();               // default order <
    void      Merge     (List<T>& list);  // merges "list" into this list
    void      Reverse   ();

    template < class Predicate > // Predicate object used to determine order
    void      Sort      (Predicate& p);

    template < class Predicate > // Predicate object used to determine order
    void      Merge     (List<T>& list, Predicate& p);

    // information about the list - accessors
    size_t    Size  () const;  // return the number of elements on the list
    bool      Empty () const;  // true iff list has no elements

    // accessing values on the list - more accessors
    T&        Front ();        // return reference to Tval at front of list
    const T&  Front () const;  // const version
    T&        Back  ();        // return reference to Tval at back of list
    const T&  Back  () const;  // const version

    // Iterator support - locating places on the list
    Iterator  Begin      (); // return iterator to front        [9]
    Iterator  End        (); // return iterator "1 past the back"
    Iterator  rBegin     (); // return iterator to back
    Iterator  rEnd       (); // return iterator "1 past the front" in reverse
    Iterator  Includes   (const T& t); // returns position of first occurrence of t, or End()

    // ConstIterator support
    ConstIterator  Begin      () const;
    ConstIterator  End        () const;
    ConstIterator  rBegin     () const;
    ConstIterator  rEnd       () const;
    ConstIterator  Includes   (const T& t) const;

    // generic display methods
    void Display (std::ostream& os, char ofc = '\0') const;
    void Dump    (std::ostream& os, char ofc = '\0') const;

    // structural integrity check
    void CheckLinks(std::ostream& os = std::cout) const;

  protected:
    // A scope List<T>:: class usable only by its friends (all members are private)
    class Link
    {
      friend class List<T>;
      friend class ConstListIterator<T>;
      friend class ListIterator<T>;

      // Link variables
      T       Tval_;        // data
      Link *  prev_;        // ptr to predecessor Link
      Link *  next_;        // ptr to successor Link

      // Link constructor - parameter required
      Link(const T& );
    } ;

    Link *  head_,  // node representing "one before the first"
         *  tail_;  // node representing "one past the last"

    // protected methods -- used only by other methods
    void Init   ();                 // sets up head and tail nodes
    void Append (const List& list); // append deep copy of list

    // protected method isolates memory allocation and associated exception handling
    static Link * NewLink (const T&);

    // standard link-in and link-out processes
    static void   LinkIn  (Link * location, Link * newLink);
    static Link * LinkOut (Link * oldLink);

    // tight couplings
    friend class ListIterator<T>;
    friend class ConstListIterator<T>; // [1]
    // numbers in square brackets refer to notes at the end of this file
  } ;

  // global scope operators and functions

  template <typename T>
  bool operator == (const List<T>& list1, const List<T>& list2);

  template <typename T>
  bool operator != (const List<T>& list1, const List<T>& list2);

  template <typename T>
  std::ostream& operator << (std::ostream& os, const List<T>& list);

  //----------------------------------
  //     ConstListIterator<T>
  //----------------------------------

  template <typename T>
  class ConstListIterator
  {
  public:
    // terminology support
    typedef T                                      ValueType;
    typedef ConstListIterator < T >                ConstIterator;
    typedef ListIterator < T >                     Iterator;

    // constructors
    ConstListIterator       ();                          // default constructor
    ConstListIterator       (const ConstListIterator& ); // copy constructor

    // information/access
    bool Valid         () const;  // cursor is valid element           [11]

    // various operators
    bool            operator == (const ConstIterator& i2) const;
    bool            operator != (const ConstIterator& i2) const;
    const T&        operator *  () const; // Return const reference to current Tval
    ConstIterator&  operator =  (const ConstIterator& i);
    ConstIterator&  operator ++ ();    // prefix
    ConstIterator   operator ++ (int); // postfix
    ConstIterator&  operator -- ();    // prefix
    ConstIterator   operator -- (int); // postfix

  protected:
    // data
    typename List<T>::Link * curr_;

    // methods
    ConstListIterator (typename List<T>::Link * linkPtr); // type converting constructor
    T& Retrieve () const; // conflicted Retrieve used by both versions of operator*

    // tight couplings
    friend class List<T>;
  } ;

  //----------------------------------
  //     ListIterator<T>
  //----------------------------------

  template <typename T>
  class ListIterator : public ConstListIterator<T>
  {
  public:
    // terminology support
    typedef T                                      ValueType;
    typedef ConstListIterator < T >                ConstIterator;
    typedef ListIterator < T >                     Iterator;

    // constructors
    ListIterator       ();                       // default constructor
    ListIterator       (List<T>& );              // initializing constructor
    ListIterator       (const ListIterator& );   // copy constructor

    // information/access
    // bool Valid         () const;

    // various operators
    // bool       operator == (const Iterator& i2) const;
    // bool       operator != (const Iterator& i2) const;
    T&         operator *  ();       // Return reference to current Tval
    const T&   operator *  () const; // const version                 [10]
    Iterator&  operator =  (const Iterator& i);
    Iterator&  operator ++ ();    // prefix
    Iterator   operator ++ (int); // postfix
    Iterator&  operator -- ();    // prefix
    Iterator   operator -- (int); // postfix

  protected:
    // methods
    ListIterator (typename List<T>::Link * linkPtr); // type converting constructor
    // T& Retrieve () const; // conflicted Retrieve used by both versions of operator*

    // tight couplings
    friend class List<T>;
  } ;

#include <list.cpp>
#include <list_sort.cpp>

} // namespace fsu

/* Notes
   =====

[1] The classes List and ListIterator are mutual friends, which gives each
    class access to the other's private and protected members. This feature is
    called "tight coupling".

[2] These definitions are very useful to client programs. For examples:

    typename fsu::List<T> x;   // declares a list x with List::ValueType = T
    typename fsu::List<T>::Iterator i;  // declares a companion iterator for x 
    typename x::ValueType t1;  // declares a variable of type T
    typename i::ValueType t2;  // another way to declare a variable of type T

    -- note that client programs cannot access "T" directly, so "ValueType" is
       essential in cases where a client program needs to name the type

[3] The error messages generated by List operations are due to

    (a) Failure to allocate new memory. These failures are handled first by the static 
        NewLink(t) method. When control is returned to the calling method:

    (b) Calls to access methods (e.g. Front(), Back()) on an empty list.

[5] Insert(i,t) returns the iterator pointing to the newly inserted element.
    Insert(i,t) places t at the end of the list when i is not valid.

[6] Insert(t) inserts in some unspecified location and returns an iterator
    pointing to the inserted element. (A good choice is PushBack(t).)

[7] Remove(i) returns the iterator pointing to the element at the position
    following the removed element.

[8] Remove(t) removes all copies of t and returns the number of elements removed.

[9] Begin() returns an iterator to the first position in the list, while End()
    returns an iterator "one past the last" position. Using the two extra links
    head_ and tail_ provides distinct valid entities to represent these two
    concepts.
    Similarly, rBegin() and rEnd() return the first element in a
    reverse order iteration (using operator--) and one past the last in reverese
    order, respectively. These are designed to make the following loops traverse
    the entire list x:

      for (i = x.Begin();  i != x.End();  ++i)  {// body}
      for (i = x.rBegin(); i != x.rEnd(); --i)  {// body}

    Note the first loop goes from front to back, the second goes from back to
    front. These loops define the *standard traversals* or *native traversals*
    (forward and reverse) of the container.

      Begin()  = first postion = Iterator(head_->next_)
      End()    = one past the last = Iterator(tail_)
      rBegin() = last postion = Iterator(tail_->prev_)
      rEnd()   = one before the first = Iterator(head_)

    Finally, note that the end iterators remain valid and can be "backed up":

      if e = End() then --e == rBegin() 
      if b = rEnd() then ++b == Begin() 

[10] operator *() is defined only when the iterator is valid

[11] "i.Valid()" can not be used in place of "i != x.End()" for traversals.

[12] Note that list.cpp and list_sort.cpp are "slave files", that is, included inside the
     namespace and multiple read protection provided by list.h. This means that
     client programs do not need to know about list.cpp - client programs only
     need to #include<list.h> in order to use List and ListIterator.
     The file list.h logically contains the implementations, even though
     physically they are in different files.

[13] Clone() is used in polymorphic programming
*/

#endif
