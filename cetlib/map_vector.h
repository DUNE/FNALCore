#ifndef CETLIB_MAP_VECTOR_H
#define CETLIB_MAP_VECTOR_H

// ======================================================================
//
// map_vector: a map mimicking a sparse vector interface
//
// Integer subscripting is unsupported and yields a compilation failure.
//
// ======================================================================

#include "cetlib/exception.h"
#include "cpp0x/algorithm"
#include <iostream>
#include <stdexcept>
#include <vector>

namespace cet  {
  class map_vector_key;

  bool  operator == ( map_vector_key const &, map_vector_key const & );
  bool  operator != ( map_vector_key const &, map_vector_key const & );
  bool  operator <  ( map_vector_key const &, map_vector_key const & );
  bool  operator >  ( map_vector_key const &, map_vector_key const & );
  bool  operator <= ( map_vector_key const &, map_vector_key const & );
  bool  operator >= ( map_vector_key const &, map_vector_key const & );

  std::ostream &  operator << ( std::ostream &, map_vector_key const & );

  template< class Value>  class map_vector;

  template< class Value >
  bool
    operator < ( typename map_vector<Value>::value_type const &
               , typename map_vector<Value>::value_type const &
               );
}

// ======================================================================

class cet::map_vector_key
{
public:
  // c'tors:
  map_vector_key( )                            : key_(-1)   { }
  explicit map_vector_key( int           key ) : key_(key)  { }
  explicit map_vector_key( unsigned      key ) : key_(key)  { }
  explicit map_vector_key( unsigned long key ) : key_(key)  { }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  // observers:
  int       asInt ( ) const  { return key_; }
  unsigned  asUint( ) const  { return key_; }

private:
  int  key_;

};  // map_vector_key

// ======================================================================

template< class Value >
  class cet::map_vector
{
public:
  // types:
  typedef  map_vector_key                              key_type;
  typedef  Value                                       mapped_type;
  typedef  std::pair<key_type,mapped_type>             value_type;
  typedef  std::vector<value_type>                     impl_type;

  typedef  typename impl_type::size_type               size_type;
  typedef  typename impl_type::difference_type         difference_type;

  typedef  typename impl_type::iterator                iterator;
  typedef  typename impl_type::const_iterator          const_iterator;
  typedef  typename impl_type::reverse_iterator        reverse_iterator;
  typedef  typename impl_type::const_reverse_iterator  const_reverse_iterator;

  typedef  typename impl_type::allocator_type          allocator_type;
  typedef  typename allocator_type::pointer            pointer;
  typedef  typename allocator_type::const_pointer      const_pointer;
  typedef  typename allocator_type::reference          reference;
  typedef  typename allocator_type::const_reference    const_reference;

  // c'tors:
  map_vector( ) : v_()  { }

  template< class InIter >
    map_vector( InIter b, InIter e ) : v_()  { insert(b, e); }

  // use compiler-generated copy c'tor, copy assignment, and d'tor

  // properties:
  bool       empty( )    const  { return v_.empty();    }
  size_type  size( )     const  { return v_.size();     }
  size_type  max_size( ) const  { return v_.max_size(); }
  size_type  capacity( ) const  { return v_.capacity(); }

  allocator_type  get_allocator( ) const  { return v_.get_allocator(); }

  // observers:
  value_type  front( ) const  { return v_.empty() ? value_type() : v_.front(); }
  value_type  back ( ) const  { return v_.empty() ? value_type() : v_.back(); }

  int  delta( ) const  { return v_.empty() ? 0 : 1 + v_.back().first.asInt(); }

  bool  has( key_type key ) const;

  iterator        find( key_type key );
  const_iterator  find( key_type key ) const;

  mapped_type       *   getOrNull( key_type key );
  mapped_type const *   getOrNull( key_type key ) const;

  mapped_type       &   getOrThrow( key_type key );
  mapped_type const &   getOrThrow( key_type key ) const;

  mapped_type       &  operator [] ( key_type key );
  mapped_type const &  operator [] ( key_type key ) const  { return getOrThrow(key); }
  mapped_type const &  at          ( key_type key ) const  { return getOrThrow(key); }

  // iterators:
  iterator        begin( )        { return v_.begin(); };
  const_iterator  begin( ) const  { return v_.begin(); }

  iterator        end( )        { return v_.end(); };
  const_iterator  end( ) const  { return v_.end(); }

  reverse_iterator        rbegin( )        { return v_.rbegin(); };
  const_reverse_iterator  rbegin( ) const  { return v_.rbegin(); }

  reverse_iterator        rend( )        { return v_.rend(); };
  const_reverse_iterator  rend( ) const  { return v_.rend(); }

  // mutators:
  void  clear( )  { v_.clear(); }

  void  reserve( size_type n )  { v_.reserve(n); }

  void  swap( map_vector<mapped_type> & other )  { v_.swap(other.v_); }

  void  push_back( value_type const & x );

  template< class InIter >
    void  insert( InIter b, InIter e );

private:
  impl_type  v_;

  bool  class_invariant( ) const;

};  // map_vector<>

// ======================================================================
// additional map_vector_key implementation

// ----------------------------------------------------------------------
// comparisons:

inline bool
  cet::operator == ( map_vector_key const & k1, map_vector_key const & k2 )
{ return k1.asInt() == k2.asInt(); }

inline bool
  cet::operator != ( map_vector_key const & k1, map_vector_key const & k2 )
{ return k1.asInt() != k2.asInt(); }

inline bool
  cet::operator <  ( map_vector_key const & k1, map_vector_key const & k2 )
{ return k1.asInt() < k2.asInt(); }

inline bool
  cet::operator >  ( map_vector_key const & k1, map_vector_key const & k2 )
{ return k1.asInt() > k2.asInt(); }

inline bool
  cet::operator <= ( map_vector_key const & k1, map_vector_key const & k2 )
{ return k1.asInt() <= k2.asInt(); }

inline bool
  cet::operator >= ( map_vector_key const & k1, map_vector_key const & k2 )
{ return k1.asInt() >= k2.asInt(); }

// ----------------------------------------------------------------------
// output:

inline std::ostream &
  cet::operator << ( std::ostream & os, cet::map_vector_key const & key )
{ return os << key.asInt(); }

// ======================================================================
// additional map_vector<> implementation

// ----------------------------------------------------------------------
// observers:

template< class Value >
bool
  cet::map_vector<Value>::has( key_type key ) const
{
  value_type  v(key, mapped_type());
  return std::binary_search(v_.begin(), v_.end(), v, operator < <Value> );
}

template< class Value >
typename cet::map_vector<Value>::iterator
  cet::map_vector<Value>::find( key_type key )
{
  value_type  v(key, mapped_type());

  iterator const begin = v_.begin()
               , end   = v_.end();

  iterator  it = std::lower_bound(begin, end, v, operator < <Value>);
  if( it != end  &&  it->first != key )
    it = end;

  return it;
}

template< class Value >
typename cet::map_vector<Value>::const_iterator
  cet::map_vector<Value>::find( key_type key ) const
{
  value_type  v(key, mapped_type());

  const_iterator const begin = v_.begin()
                     , end   = v_.end();

  const_iterator  it = std::lower_bound(begin, end, v, operator < <Value>);
  if( it != end  &&  it->first != key )
    it = end;

  return it;
}

template< class Value >
Value *
  cet::map_vector<Value>::getOrNull( key_type key )
{
  iterator  it = find(key);
  return it == v_.end()  ?  nullptr  :  & it->second;
}

template< class Value >
Value const *
  cet::map_vector<Value>::getOrNull( key_type key ) const
{
  const_iterator  it = find(key);
  return it == v_.end()  ?  nullptr  :  & it->second;
}

template< class Value >
Value &
  cet::map_vector<Value>::getOrThrow( key_type key )
{
  Value *  p = getOrNull(key);
  if( p == nullptr )
    throw cet::exception("map_vector::getOrThrow")
      << "out of range (no such key): " << key.asInt()
      << std::endl;

  return *p;
}

template< class Value >
Value const &
  cet::map_vector<Value>::getOrThrow( key_type key ) const
{
  Value const *  p = getOrNull(key);
  if( p == nullptr )
    throw cet::exception("map_vector::getOrThrow")
      << "out of range (no such key): " << key.asInt()
      << std::endl;

  return *p;
}

template< class Value >
Value &
  cet::map_vector<Value>::operator [] ( key_type key )
{
  value_type  v(key, mapped_type());

  iterator const begin = v_.begin()
               , end   = v_.end();

  iterator  it = std::lower_bound(begin, end, v, operator < <Value>);
  if( it == end  ||  it->first != key )
    it = v_.insert(it, v);

  return it->second;
}

// ----------------------------------------------------------------------
// mutators:

template< class Value >
void
  cet::map_vector<Value>::push_back( value_type const & x )
{
  v_.push_back( std::make_pair(x.first.asInt() + delta(), x.second) );
}

template< class Value >
template< class InIter >
void
  cet::map_vector<Value>::insert( InIter b, InIter e )
{
  int d = delta();
  for(  ;  b != e;  ++b )
    v_.push_back( std::make_pair(b->first.asInt() + d, b->second) );
}

// ----------------------------------------------------------------------
// helpers:

template< class Value >
bool
  cet::map_vector<Value>::class_invariant( ) const
{
  return std::is_sorted(v_.begin(), v_.end(), operator < <Value> );
}

// ======================================================================

template< class Value >
bool
  cet::operator < ( typename map_vector<Value>::value_type const & v1
                  , typename map_vector<Value>::value_type const & v2
                  )
{
  return v1.first < v2.first;
}

// ======================================================================

#endif
