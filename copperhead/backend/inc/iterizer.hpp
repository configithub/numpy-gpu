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
#include "node.hpp"
#include "type.hpp"
#include "ctype.hpp"
#include "cppnode.hpp"
#include "rewriter.hpp"
#include "import/library.hpp"
#include <set>
#include <map>
#include <stdexcept>

namespace backend {

/*!
  \addtogroup rewriters
  @{
*/

//! A rewrite pass which converts tail recursion into iteration
class iterizer
    : public rewriter<iterizer>
{
public:
    using rewriter<iterizer>::operator();
    result_type operator()(const procedure& n);
};

}
