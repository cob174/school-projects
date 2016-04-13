/*
    list.cpp
    01/22/2014
    Chris Lacher

    slave file to list.h

    Implementation of List<T> API
    except for List::Sort which is in a separate slave file

    Fall 2013:   made predicate objects non-const to use "spyware".
    Spring 2014: upgraded to C++11 "nullptr"

    Copyright 2014, R. C. Lacher
*/

//----------------------------------
//     List<T>:: Implementations
//----------------------------------

// macroscopic mutators (except for sorts)

template < typename T >
template < class P >
void List<T>::Merge (List<T>& y, P& p)
// merges x into this list; post: true = x.Empty()
// if both lists are ordered result is ordered
{
  if (this == &y) return;
  Link * xcurr = head_->next_;   // x = this list
  Link * ycurr = y.head_->next_; // y = that list

  // merge while both lists are not empty
  while (xcurr != tail_ && ycurr != y.tail_)
  {
    if (p(ycurr->Tval_,xcurr->Tval_)) // y < x
    {
      ycurr = ycurr->next_;
      LinkIn(xcurr,ycurr->prev_);
    }
    else // x <= y
    {
      xcurr = xcurr->next_;
    }
  }

  // splice in remainder of y
  if (ycurr != y.tail_)
  {
    tail_->prev_->next_ = ycurr;
    ycurr->prev_ = tail_->prev_;
    tail_->prev_ = (y.tail_)->prev_;
    (y.tail_)->prev_->next_ = tail_;
  }

  // make y structurally correct for empty
  (y.head_)->next_ = y.tail_;
  (y.tail_)->prev_ = y.head_;
}

template < typename T >
void List<T>::Merge (List<T>& y )
{
  if (this == &y) return;
  Link * xcurr = head_->next_;   // x = this list
  Link * ycurr = y.head_->next_; // y = that list

  // merge while both lists are not empty
  while (xcurr != tail_ && ycurr != y.tail_)
  {
    if (ycurr->Tval_ < xcurr->Tval_) // y < x
    {
      ycurr = ycurr->next_;
      LinkIn(xcurr,ycurr->prev_);
    }
    else // x <= y
    {
      xcurr = xcurr->next_;
    }
  }

  // splice in remainder of y
  if (ycurr != y.tail_)
  {
    tail_->prev_->next_ = ycurr;
    ycurr->prev_ = tail_->prev_;
    tail_->prev_ = (y.tail_)->prev_;
    (y.tail_)->prev_->next_ = tail_;
  }

  // make y structurally correct for empty
  (y.head_)->next_ = y.tail_;
  (y.tail_)->prev_ = y.head_;
}

template < typename T >
void List<T>::Reverse ()
{
  if (head_->next_ == tail_)
    return;

  // swap prev_ and next_ for each link
  typename List<T>::Link * link(head_), * temp(nullptr);
  while (link != nullptr)
  {
    temp        = link->next_;
    link->next_ = link->prev_;
    link->prev_ = temp;
    link        = temp; // move to "old next" = link->prev_
  }
  // swap head_ and tail_
  temp  = head_;
  head_ = tail_;
  tail_ = temp;
}

// end macromutators */

// stand-alone functions

template < typename T >
bool operator == (const List<T>& x1, const List<T>& x2)
{
  typename List<T>::ConstIterator i1, i2;
  for (
        i1 = x1.Begin(), i2 = x2.Begin();
        (i1 != x1.End()) && (i2 != x2.End());
        ++i1, ++i2
      )
  {
    if (*(i1) != *(i2))
      return 0;
  }
  if (i1 != x1.End() || i2 != x2.End())
    return 0;
  return 1;
}

template < typename T >
bool operator != (const List<T>& x1, const List<T>& x2)
{
  return !(x1 == x2);
}

template < typename T >
std::ostream& operator << (std::ostream& os, const List<T>& x2)
{
  x2.Display(os);
  return os;
}

// end stand-alone functions */

// memory allocator and other private methods

template < typename T >
List<T>::Link::Link (const T& Tval) : Tval_(Tval), prev_(nullptr), next_(nullptr)
// Link constructor
{}

template < typename T >
typename List<T>::Link* List<T>::NewLink (const T& t)
{
  Link * newLink = new(std::nothrow) Link (t);
  if (nullptr == newLink)
  { 
    // exception handler
    std::cerr << "** List error: memory allocation failure\n"; 
    return nullptr;
  }
  return newLink;
}

template < typename T >
void List<T>::LinkIn(typename List<T>::Link * location, typename List<T>::Link * newLink)
// link newLink into list at (ahead of) location
{
  newLink->next_ = location;
  newLink->prev_ = location->prev_;
  newLink->next_->prev_ = newLink;
  newLink->prev_->next_ = newLink;
}

template < typename T >
typename List<T>::Link * List<T>::LinkOut(typename List<T>::Link * oldLink)
// unlink oldLink from list, return oldLink
{
  oldLink->prev_->next_ = oldLink->next_;
  oldLink->next_->prev_ = oldLink->prev_;
  oldLink->prev_ = nullptr;
  oldLink->next_ = nullptr;
  return oldLink;
}

template < typename T >
void List<T>::Init()
// used by constructors
{
  head_ = NewLink(T());
  tail_ = NewLink(T());
  head_->next_ = tail_;
  tail_->prev_ = head_;
}

template < typename T >
void List<T>::Append(const List<T>& list)
// Appends deep copy of rhs - do not apply to self!
{
  for (ConstIterator i = list.Begin(); i != list.End(); ++i)
    PushBack(*i);
}

// end private methods */

// constructors and assignment

template < typename T >
List<T>::List ()  :  head_(nullptr), tail_(nullptr)
// default constructor
{
  Init();
}

template < typename T >
List<T>::List (const List<T>& x) : head_(nullptr), tail_(nullptr)
// copy constructor
{
  Init();
  Append(x);
}

template < typename T >
List<T>::~List ()
// destructor
{
  Clear();
  delete head_;
  delete tail_;
} 

template < typename T >
List<T>& List<T>::operator = (const List<T>& rhs)
// assignment operator
{
  if (this != &rhs)   // it would be a disaster to assign to self!
  {
    // First, destroy the existing list.
    Clear();
    // then build a copy as *this
    Append(rhs);
  }
  return *this;
}

template < typename T >
List<T>& List<T>::operator += (const List<T>& list)
// append operator
{
  if (this != &list)
    Append(list);
  return *this;
}

template < typename T >
bool List<T>::PushFront (const T& t)
// Insert t at the front (first) position.
{
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return 0;
  LinkIn(head_->next_,newLink);
  return 1;
}

/*
{
  // 1. create new element
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return 0;
  // set location for insertion
  Link* location = head_->next_;
  // 3. link new element into the list and return
  newLink->next_ = location;
  newLink->prev_ = location->prev_;
  newLink->next_->prev_ = newLink;
  newLink->prev_->next_ = newLink;
  return 1;
}
*/

template < typename T >
bool List<T>::PushBack (const T& t)
// Insert t at the back (last) position.
{
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return 0;
  LinkIn(tail_,newLink);
  return 1;
}

/*
{
  // 1. create new element
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return 0;
  // set location for insertion
  Link* location = tail_;
  // 3. link new element into the list and return
  newLink->next_ = location;
  newLink->prev_ = location->prev_;
  newLink->next_->prev_ = newLink;
  newLink->prev_->next_ = newLink;
  return 1;
}
*/

template < typename T >
ListIterator<T> List<T>::Insert (ListIterator<T> i, const T& t)
// Insert t at (in front of) i; return i at new element
{
  if (Empty())  // always insert 
  {
    i = End();
  }
  if (!i.Valid() || i == rEnd()) // null or off-the-front
  {
    std::cerr << " ** cannot insert at position -1\n";
    return End();
  }
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return End();
  LinkIn(i.curr_,newLink);
  // leave i at new entry and return
  i.curr_ = newLink;
  return i;
}

template < typename T >
ConstListIterator<T> List<T>::Insert (ConstListIterator<T> i, const T& t)
// Insert t at (in front of) i; return i at new element
{
  if (Empty())  // always insert 
  {
    i = End();
  }
  if (!i.Valid() || i == rEnd()) // null or off-the-front
  {
    std::cerr << " ** cannot insert at position -1\n";
    return End();
  }
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return End();
  LinkIn(i.curr_,newLink);
  // leave i at new entry and return
  i.curr_ = newLink;
  return i;
}

/*
{
  // 1. create new element
  Link* newLink = NewLink(t);
  if (newLink == nullptr) return End();
  // 2. link new element into the list
  newLink->next_ = i.curr_;
  newLink->prev_ = i.curr_->prev_;
  newLink->next_->prev_ = newLink;
  newLink->prev_->next_ = newLink;
  // 3. leave i at new entry and return
  i.curr_ = newLink;
  return i;
} // end Insert((i,t))
*/

template < typename T >
ListIterator<T> List<T>::Insert  (const T& t)
// Insert t at default location (back)
{
  return Insert(End(),t);
}

template < typename T >
bool List<T>::PopFront()
{
  if (Empty())
  {
    std::cerr << "** List error: PopFront() called on empty list\n"; 
    return 0;
  }
  Link * oldLink = LinkOut(head_->next_);
  delete oldLink;
  return 1;
} // end PopFront()

template < typename T >
bool List<T>::PopBack()
{
  if (Empty())
  {
    std::cerr << "** List error: PopBack() called on empty list\n"; 
    return 0;
  }
  Link* oldLink = LinkOut(tail_->prev_);
  delete oldLink;
  return 1;
} // end PopBack()

template < typename T >
ListIterator<T> List<T>::Remove (Iterator i)
// Remove item at i
{
  // first deal with the impossible cases
  if (i.curr_ == nullptr || i.curr_ == head_ || i.curr_ == tail_)
  {
    std::cerr << "** List error: Remove(i) called with vacuous iterator\n"; 
    return i;
  }
  i.curr_ = i.curr_->next_;                  // advance iterator
  Link * oldLink = LinkOut(i.curr_->prev_);  // unlink element to be removed
  delete oldLink;                            // delete 
  return i;                                  // return i at new position
} // end Remove(Iterator)

template < typename T >
ConstListIterator<T> List<T>::Remove (ConstIterator i)
// Remove item at i
{
  // first deal with the impossible cases
  if (i.curr_ == nullptr || i.curr_ == head_ || i.curr_ == tail_)
  {
    std::cerr << "** List error: Remove(i) called with vacuous iterator\n"; 
    return i;
  }
  i.curr_ = i.curr_->next_;                  // advance iterator
  Link * oldLink = LinkOut(i.curr_->prev_);  // unlink element to be removed
  delete oldLink;                            // delete 
  return i;                                  // return i at new position
} // end Remove(Iterator)

template < typename T >
size_t  List<T>::Remove (const T& t)
// Remove all copies of t
// This doesn't use member priviledge, so could be an external function
// It gives the archetype for interaction between Remove(i) and ++i
{
  size_t count(0);
  Iterator i = Begin();
  while (i != End())
  {
    if (t == *i)
    {
      i = Remove(i);
      ++count;
    }
    else
    {
      ++i;
    }
  }
  return (count);
}  // end Remove(t)

template < typename T >
void List<T>::Clear()
// Deletes all elements of the list
{
  while (!Empty())
    PopFront();
} // end Clear()

template < typename T >
List<T> * List<T>::Clone() const
// returns pointer to a copy of *this
{
  List * clone = new List;
  clone->Append(*this);
  return clone;
} 

template < typename T >
size_t List<T>::Size()  const
{
  size_t  size(0);
  Link * curr(head_->next_);
  while (curr != tail_)
  {
    curr = curr -> next_;
    ++size;
  }
  return size;
}

template < typename T >
bool List<T>::Empty()  const
{
  return (head_->next_ == tail_);
}

template < typename T >
T& List<T>::Front ()
{
  if (Empty())
  {
      std::cerr << "** List error: Front() called on empty list\n"; 
  }
  return *Begin();
}

template < typename T >
const T& List<T>::Front () const
{
  if (Empty())
  {
      std::cerr << "** List error: Front() called on empty list\n"; 
  }
  return *Begin();
}

template < typename T >
T& List<T>::Back  ()
{
  if (Empty())
  {
      std::cerr << "** List error: Back() called on empty list\n"; 
  }
  return *rBegin();
}

template < typename T >
const T& List<T>::Back  () const
{
  if (Empty())
  {
      std::cerr << "** List error: Back() called on empty list\n"; 
  }
  return *rBegin();
}

template <typename T> 
ListIterator<T>  List<T>::Includes (const T& t)
// sequential search
{
  Iterator i;
  for (i = Begin(); i != End(); ++i)
  {
    if (t == *i)
      return i;
  }
  return End();
} // Includes()

// output methods

template < typename T >
void List<T>::Display (std::ostream& os, char ofc) const
{
  ConstIterator i;
  if (ofc == '\0')
    for (i = Begin(); i != End(); ++i)
      os << *i;
  else
    for (i = Begin(); i != End(); ++i)
      os << *i << ofc;
} // Display()

// debug/developer methods

template < typename T >
void List<T>::Dump (std::ostream& os, char ofc) const
{
  Display(os,ofc);
  os << '\n';
} // Dump()

template < typename T >
void List<T>::CheckLinks(std::ostream& os) const
{
  typename List<T>::ConstIterator i,j,k;
  // bool ok = 1;
  size_t n = 0;
  for (i = this->Begin(), n = 0; i != this->End(); ++i, ++n)
  {
    j = i; --j; ++j;
    k = i; ++k; --k;
    if (!i.Valid() || !j.Valid() || !k.Valid() || i != j || i != k)
    {
      os << " ** forward reciprocity failure at position " << n << '\n';
      // ok = 0;
      break;
    }
  }
  // if (ok) os << " ** passed forward reciprocity check\n";
  
  for (i = this->rBegin(), n = Size(); i != this->rEnd(); --i, --n)
  {
    j = i; --j; ++j;
    k = i; ++k; --k;
    if (!i.Valid() || !j.Valid() || !k.Valid() || i != j || i != k)
    {
      os << " ** reverse reciprocity failure at position " << n << '\n';
      // ok = 0;
      break;
    }
  }     
  // if (ok) os << " ** passed reverse reciprocity check\n";
} // CheckLinks()

// Iterator support

template < typename T >
ListIterator<T>  List<T>::Begin()
{
  Iterator i(head_->next_);
  return i;
}

template < typename T >
ListIterator<T>  List<T>::End()
{
  Iterator i(tail_);
  return i;
}

template < typename T >
ListIterator<T>  List<T>::rBegin()
{
  Iterator i(tail_->prev_);
  return i;
}

template < typename T >
ListIterator<T>  List<T>::rEnd ()
{
  Iterator i(head_);
  return i;
}

// ConstIterator support

template < typename T >
typename List<T>::ConstIterator  List<T>::Begin() const
{
  ConstIterator i(head_->next_);
  return i;
}

template < typename T >
typename List<T>::ConstIterator  List<T>::End() const
{
  ConstIterator i(tail_);
  return i;
}

template < typename T >
typename List<T>::ConstIterator  List<T>::rBegin() const
{
  ConstIterator i(tail_->prev_);
  return i;
}

template < typename T >
typename List<T>::ConstIterator  List<T>::rEnd () const
{
  ConstIterator i(head_);
  return i;
}

template <typename T> 
typename List<T>::ConstIterator  List<T>::Includes (const T& t) const
{
  ConstIterator i;
  for (i = Begin(); i != End(); ++i)
  {
    if (t == *i)
      return i;
  }
  return End();
} // Includes()

//-------------------------------------------
//     ConstListIterator<T>:: Implementations
//-------------------------------------------

// default constructor
template < typename T >
ConstListIterator<T>::ConstListIterator ()  :  curr_(nullptr)
// construct a null ConstListIterator 
{}

// protected constructor
template < typename T >
ConstListIterator<T>::ConstListIterator (typename List<T>::Link* link) : curr_(link)
// construct an iterator around a link pointer (not available to client programs)
{}

// copy constructor
template < typename T >
ConstListIterator<T>::ConstListIterator (const ConstListIterator<T>& i)
   :  curr_(i.curr_)
{}

// assignment operator
template < typename T >
ConstListIterator<T>& ConstListIterator<T>::operator = (const ConstListIterator <T> & i)
{
   curr_ = i.curr_;
   return *this;
}

// protected method
template < typename T >
T&  ConstListIterator<T>::Retrieve() const
// Return reference to current t
// note conflicted signature - const method returns non-const reference
{
  if (curr_ == nullptr)
  {
    std::cerr << "** Error: ConstListIterator<T>::Retrieve() invalid dereference\n";
    exit (EXIT_FAILURE);
  }
  return curr_->Tval_;
}

template < typename T >
bool ConstListIterator<T>::Valid() const
// test cursor for legal dereference
{
  return curr_ != nullptr;
}

template < typename T >
bool ConstListIterator<T>::operator == (const ConstListIterator<T>& i2) const
{
  if (curr_ == i2.curr_)
    return 1;
  return 0;
}

template < typename T >
bool ConstListIterator<T>::operator != (const ConstListIterator<T>& i2) const
{
  return !(*this == i2);
}

template < typename T >
const T&  ConstListIterator<T>::operator * () const
{
  return Retrieve();
}

template < typename T >
ConstListIterator<T>& ConstListIterator<T>::operator ++ ()
// prefix increment
{
  if (curr_ != nullptr)
    curr_ = curr_->next_;
  return *this;
}

template < typename T >
ConstListIterator<T> ConstListIterator<T>::operator ++ (int)
// postfix increment
{
   ConstListIterator <T> clone = *this;
   this->operator++();
   return clone;
}

template < typename T >
ConstListIterator<T>& ConstListIterator<T>::operator -- ()
// prefix decrement
{
  if (curr_ != nullptr)
    curr_ = curr_->prev_;
  return *this;
}

template < typename T >
ConstListIterator<T> ConstListIterator<T>::operator -- (int)
// postfix decrement
{
   ConstListIterator <T> clone = *this;
   this->operator--();
   return clone;
}

//-------------------------------------------
//     ListIterator<T>:: Implementations
//-------------------------------------------

// default constructor
template < typename T >
ListIterator<T>::ListIterator ()  :  ConstListIterator<T>()
// construct a null ListIterator 
{}

// protected constructor
template < typename T >
ListIterator<T>::ListIterator (typename List<T>::Link* link) : ConstListIterator<T>(link)
// construct an iterator around a link pointer (not available to client programs)
{}

// copy constructor
template < typename T >
ListIterator<T>::ListIterator (const ListIterator<T>& i)
   :  ConstListIterator<T>(i)
{}

// assignment operator
template < typename T >
ListIterator<T>& ListIterator<T>::operator = (const ListIterator <T> & i)
{
  ConstListIterator<T>::operator=(i);
  return *this;
}

template < typename T >
const T&  ListIterator<T>::operator * () const
{
  return ConstListIterator<T>::Retrieve();
}

template < typename T >
T&  ListIterator<T>::operator * ()
{
  return ConstListIterator<T>::Retrieve();
}

template < typename T >
ListIterator<T>& ListIterator<T>::operator ++ ()
// prefix increment
{
  ConstListIterator<T>::operator++();
  return *this;
}

template < typename T >
ListIterator<T> ListIterator<T>::operator ++ (int)
// postfix increment
{
   ListIterator <T> clone = *this;
   this->operator++();
   return clone;
}

template < typename T >
ListIterator<T>& ListIterator<T>::operator -- ()
// prefix decrement
{
  ConstListIterator<T>::operator--();
  return *this;
}

template < typename T >
ListIterator<T> ListIterator<T>::operator -- (int)
// postfix decrement
{
   ListIterator <T> clone = *this;
   this->operator--();
   return clone;
}

