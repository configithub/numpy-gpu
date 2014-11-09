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
#pragma once

#include <thrust/scan.h>
#include <thrust/device_vector.h>
#include <thrust/iterator/reverse_iterator.h>
#include <prelude/runtime/make_cuarray.hpp>
#include <prelude/runtime/make_sequence.hpp>
#include <prelude/runtime/tags.h>
#include <thrust/iterator/retag.h>
#include <prelude/primitives/stored_sequence.h>

namespace copperhead {

template<typename F, typename Seq>
sp_cuarray
scan(const F& fn, Seq& x) {
    typedef typename F::result_type T;
    typedef typename Seq::tag Tag;
    typedef typename detail::stored_sequence<Tag, T>::type sequence_type;
    boost::shared_ptr<cuarray> result_ary = make_cuarray<T>(x.size());
    sequence_type result =
        make_sequence<sequence_type>(result_ary,
                                     Tag(),
                                     true);
   
    thrust::inclusive_scan(x.begin(),
                           x.end(),
                           result.begin(),
                           fn);
    return result_ary;
}

template<typename F, typename Seq>
sp_cuarray
rscan(const F& fn, Seq& x) {
    typedef typename F::result_type T;
    typedef typename Seq::tag Tag;
    typedef typename thrust::reverse_iterator<typename Seq::iterator_type> iterator_type;
    iterator_type drbegin(x.end());
    iterator_type drend(x.begin());
    typedef typename detail::stored_sequence<Tag, T>::type sequence_type;
    boost::shared_ptr<cuarray> result_ary = make_cuarray<T>(x.size());
    sequence_type result =
        make_sequence<sequence_type>(result_ary,
                                     Tag(),
                                     true);
    thrust::reverse_iterator<thrust::pointer<T, Tag> > orbegin(result.end());
    thrust::inclusive_scan(drbegin, drend, orbegin, fn);
    return result_ary;
}

template<typename F, typename Seq>
sp_cuarray
exclusive_scan(const F& fn, const typename Seq::value_type& p, Seq& x) {
    typedef typename F::result_type T;
    typedef typename Seq::tag Tag;
    typedef typename detail::stored_sequence<Tag, T>::type sequence_type;
    boost::shared_ptr<cuarray> result_ary = make_cuarray<T>(x.size());
    sequence_type result =
        make_sequence<sequence_type>(result_ary,
                                     Tag(),
                                     true);
   
    thrust::exclusive_scan(x.begin(),
                           x.end(),
                           result.begin(),
                           p,
                           fn);
    return result_ary;
}

template<typename F, typename Seq>
sp_cuarray
exclusive_rscan(const F& fn, const typename Seq::value_type& p, Seq& x) {
    typedef typename F::result_type T;
    typedef typename Seq::tag Tag;
    typedef typename thrust::reverse_iterator<typename Seq::iterator_type> iterator_type;
    iterator_type drbegin(x.end());
    iterator_type drend(x.begin());
    typedef typename detail::stored_sequence<Tag, T>::type sequence_type;
    boost::shared_ptr<cuarray> result_ary = make_cuarray<T>(x.size());
    sequence_type result =
        make_sequence<sequence_type>(result_ary,
                                     Tag(),
                                     true);
    thrust::reverse_iterator<thrust::pointer<T, Tag> > orbegin(result.end());
    thrust::exclusive_scan(drbegin, drend, orbegin, p, fn);
    return result_ary;
}

}
