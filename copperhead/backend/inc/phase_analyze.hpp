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
#include <string>
#include <map>
#include <vector>
#include "node.hpp"
#include "rewriter.hpp"
#include "builtins/phase.hpp"
#include "environment.hpp"
#include "import/library.hpp"
#include "utility/isinstance.hpp"
#include "utility/up_get.hpp"
#include "utility/snippets.hpp"
#include "utility/markers.hpp"
#include "utility/initializers.hpp"

namespace backend {

/*! 
  \addtogroup rewriters
  @{
 */

//! A rewrite pass that adds phase boundaries
/*! This rewriter analyzes functions to determine where synchronization
  points occur, and then inserts phase boundaries to effectuate them.

  It does not do any scheduling to minimize the number of phases, which
  will be done in a future version.
*/
class phase_analyze
    : public rewriter<phase_analyze> {
private:
    const std::string m_entry_point;
    bool m_in_entry;
    std::map<std::string, std::shared_ptr<const phase_t> > m_fns;
    environment<std::string, completion> m_completions;
    environment<std::string, std::shared_ptr<const name> > m_substitutions;
    std::vector<std::shared_ptr<const statement> > m_pre_boundaries;
    std::shared_ptr<const statement> m_post_boundary;
    std::set<std::string> m_returns;
    completion m_result_completion;
    bool add_phase_boundary(const name& n, bool post=false);
    bool add_phase_boundary_tuple(const name& n, bool post=false);
    result_type make_tuple_analyze(const bind& n);
    environment<std::string,
                std::vector<std::shared_ptr<const literal> > > m_tuples;
    result_type form_suite(const std::shared_ptr<const statement>&);
public:
    using rewriter<phase_analyze>::operator();
    //! Constructor
/*! 
  
  \param entry_point The name of the entry_point function
  \param reg The registry of functions the compiler knows about
*/
    phase_analyze(const std::string& entry_point, const registry& reg);
    result_type operator()(const procedure& n);
    result_type operator()(const apply& n);
    result_type operator()(const bind& n);
    result_type operator()(const ret& n);
    result_type operator()(const closure& n);
    
};

/*! 
  @}
 */

}
