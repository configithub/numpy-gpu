/*
 *   Copyright 2012      NVIDIA Corporation
 * 
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 * 
 *       http://www.apache.org/licenses/LICENSE-2.0
 * 
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 * 
 */
/*!
  \file initializers.hpp

  \brief This file contains definitions for initializers for standard
  containers which in C++0x operate on initializer lists, offering
  functions to construct standard containers in convenient ways
  without explicit initializer lists. This is a workaround for
  important compilers, (ie Visual Studio) which don't support
  initializer lists.
  
*/
#pragma once
#include <vector>
#include <map>
#include <set>

namespace backend {
namespace utility {

/*!
  \addtogroup utilities
  @{
*/

//! Wrapper for std::vector, used to chain insertions
template<typename E>
struct vector_wrapper
    : public std::vector<E> {
    vector_wrapper<E>&& operator()(const E& input) {
        this->push_back(input);
        return std::move(*this);
    }
};

//! Convenience function for constructing std::vector objects
/*! 
  \tparam E Type of element which resulting vector will hold

  \param in The first element of the resulting vector
  
  \return A vector_wrapper<E> object, which can be used to construct
  std::vector objects.

  Use this function as follows:
  \verbatim make_vector<int>(1)(2)(3) \endverbatim
  This will construct a std::vector<int> that holds [1, 2, 3].
  Although this isn't as convenient as
  \verbatim vector<int>{1, 2, 3} \endverbatim
  It works in a similar way.  And most importantly, compilers which
  don't support initializer lists should still compile this.

*/
template<typename E>
vector_wrapper<E> make_vector(const E& in) {
    return vector_wrapper<E>()(in);
}
//! Convenience function for constructing std::vector objects
/*! 
  \tparam E Type of element which resulting vector will hold
  
  \return An empty vector_wrapper<E> object.

  This overload is provided mostly for symmetry's sake, for the case
  where you want to make an empty vector.
*/
template<typename E>
vector_wrapper<E> make_vector() {
    return vector_wrapper<E>();
}


//! Wrapper for std::map, used to chain insertions
template<typename K, typename V>
struct map_wrapper
    : public std::map<K, V> {
    map_wrapper<K, V>&& operator()(const K& key, const V& val) {
        this->insert(std::make_pair(key, val));
        return std::move(*this);
    }
};

//! Convenience function for constructing \p std::map objects
/*! 
  \tparam K Type of key for the \p std::map
  \tparam V Type of value for the \p std::map
  \param k Key for the first object in the map
  \param v Value for the first object in the map
  
  \return A map_wrapper<K, V> object, which can be used to construct
  \p std::map objects.

  Use this function as follows:
  \verbatim make_map<int, float>(1, 2.1)(2, 4.2) \endverbatim
  This will construct a \p std::map<int, float> where 1 -> 2.1, 2 -> 4.2
  Although this isn't as convenient as using an initializer list,
  it works in a similar way.  And most importantly, compilers which
  don't support initializer lists should still compile this.

*/
template<typename K, typename V>
map_wrapper<K, V> make_map(const K& k, const V& v) {
    return map_wrapper<K, V>()(k, v);
}

//! Convenience function for constructing std::map objects
/*! 
  \tparam K Type of key for the \p std::map
  \tparam V Type of value for the \p std::map
  
  \return An empty map_wrapper<K, V> object.

  This overload is provided mostly for symmetry's sake, for the case
  where you want to make an empty map.
*/
template<typename K, typename V>
map_wrapper<K, V> make_map() {
    return map_wrapper<K, V>();
}

//! Wrapper for std::set, used to chain insertions
template<typename V>
struct set_wrapper
    : public std::set<V> {
    set_wrapper<V>&& operator()(const V& val) {
        this->insert(val);
        return std::move(*this);
    }
};

//! Convenience function for constructing \p std::set objects
/*! 
  \tparam V Type of value for the \p std::set
  \param v Value for the first object in the set
  
  \return A set_wrapper<V> object, which can be used to construct
  \p std::set objects.

  Use this function as follows:
  \verbatim make_set<int>(1)(2) \endverbatim
  This will construct a \p std::set<int> containing 1 and 2.
  Although this isn't as convenient as using an initializer list,
  it works in a similar way.  And most importantly, compilers which
  don't support initializer lists should still compile this.

*/
template<typename V>
set_wrapper<V> make_set(const V& v) {
    return set_wrapper<V>()(v);
}

//! Convenience function for constructing \p std::set objects
/*! 
  \tparam V Type of value for the \p std::set
  
  \return An empty set_wrapper<K, V> object.

  This overload is provided mostly for symmetry's sake, for the case
  where you want to make an empty set.
*/
template<typename V>
set_wrapper<V> make_set() {
    return set_wrapper<V>();
}



/*!
  @}
*/

}
}
