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
#include <prelude/sequences/shifted_sequence.h>
#include <prelude/basic/detail/signed_index_type.h>
namespace copperhead {

template<typename Seq>
shifted_sequence<Seq> shift(const Seq& src,
                            const typename detail::signed_index_type<
                            typename Seq::index_type >::type& amount,
                            const typename Seq::value_type& boundary) {
    return shifted_sequence<Seq>(src, amount, boundary);
}

}
