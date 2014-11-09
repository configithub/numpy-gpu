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
#include "containerize.hpp"
#include "utility/initializers.hpp"
#include "utility/container_type.hpp"

using std::string;
using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using backend::utility::make_vector;
using std::vector;

#include "repr_printer.hpp"


namespace backend {

containerize::containerize(const string& entry_point) : m_entry_point(entry_point), m_in_entry(false) {}


containerize::result_type containerize::operator()(const name &n) {
    m_decl_containers.insert(n.id());
    return n.ptr();
}

containerize::result_type containerize::operator()(const procedure &s) {
    m_in_entry = (s.id().id() == m_entry_point);
    m_decl_containers.begin_scope();
    containerize::result_type r = this->rewriter::operator()(s);
    m_decl_containers.end_scope();
    return r;
}

shared_ptr<const expression> containerize::container_args(const expression& t) {
    if (detail::isinstance<name>(t)) {
        if (detail::container_type(t.ctype()) != t.ctype().ptr()) {
            const name& n = boost::get<const name&>(t);
            return make_shared<const name>(
                detail::wrap_array_id(n.id()),
                n.type().ptr(),
                n.ctype().ptr());
        }
    } else if (detail::isinstance<tuple>(t)) {
        bool match = true;
        vector<shared_ptr<const expression> > sub_exprs;
        const tuple& tup = boost::get<const tuple&>(t);
        for(auto i = tup.begin(); i != tup.end(); i++) {
            shared_ptr<const expression> sub = container_args(*i);
            sub_exprs.push_back(sub);
            match = match && (sub == i->ptr());
        }
        if (match) {
            return t.ptr();
        }
        return make_shared<const tuple>(
            std::move(sub_exprs),
            t.type().ptr(),
            t.ctype().ptr());
    }
    return t.ptr();
}

containerize::result_type containerize::reassign(const bind &n) {
    //LHS must be a name
    assert(detail::isinstance<name>(n.lhs()));
    const name& lhs = boost::get<const name&>(n.lhs());
    //RHS must be a name
    assert(detail::isinstance<name>(n.rhs()));
    const name& rhs = boost::get<const name&>(n.rhs());
    //Both containers must exist
    if (!m_decl_containers.exists(detail::wrap_array_id(lhs.id())) ||
        !m_decl_containers.exists(detail::wrap_array_id(rhs.id()))) {
        return n.ptr();
    }
    shared_ptr<const name> lhs_cont =
        make_shared<const name>(
            detail::wrap_array_id(lhs.id()),
            lhs.type().ptr(),
            detail::container_type(lhs.ctype()));
    shared_ptr<const name> rhs_cont =
        make_shared<const name>(
            detail::wrap_array_id(rhs.id()),
            rhs.type().ptr(),
            detail::container_type(rhs.ctype()));
    vector<shared_ptr<const statement> > stmts;
    stmts.push_back(
        make_shared<const bind>(
            lhs_cont, rhs_cont));
    stmts.push_back(n.ptr());
    return make_shared<const suite>(std::move(stmts));
           
}

containerize::result_type containerize::operator()(const bind &n) {
    if (!m_in_entry) {
        return n.ptr();
    } else {

        //Record name in lhs
        boost::apply_visitor(*this, n.lhs());
        
        const expression& rhs = n.rhs();

        if (detail::isinstance<name>(rhs)) {
            return reassign(n);
        }
        
        if (!detail::isinstance<apply>(rhs)) {
            return n.ptr();
        }
        const apply& rhs_apply = boost::get<const apply&>(rhs);
        
        //Currently, we only process formation of tuples containing sequences
        //These will always be in the form of an apply of thrust::make_tuple
        const name& apply_fn = rhs_apply.fn();
        if (apply_fn.id() != detail::snippet_make_tuple()) {
            return n.ptr();
        }

        //Does this make_tuple need to be containerized?
        vector<shared_ptr<const ctype::type_t> > arg_c_types;
        for(auto i = rhs_apply.args().begin(); i != rhs_apply.args().end(); i++) {
            arg_c_types.push_back(i->ctype().ptr());
        }
            
            
        shared_ptr<const ctype::type_t> cont_type = detail::container_type(
            *make_shared<const ctype::tuple_t>(std::move(arg_c_types)));
        //If the container is the same as the ctype, no - because this
        //means that no containers were necessary for the original
        //e.g. a tuple of scalars
        if (cont_type == n.lhs().ctype().ptr()) {
            return n.ptr();
        }
        //If any of the containers needed to construct this tuple are not
        //extant, no - this means that we're creating a tuple from
        //temporary sequences that won't ever be returned.

        //It is the responsibility of phase analysis to realize
        //temporary results as containers when an entry point is
        //returning.  This means we can assume all externally visible
        //containers will have their input containers materialized at
        //this point in the compiler.
        shared_ptr<const expression> p_cont_args_expr = container_args(rhs_apply.args());
        const expression& cont_args_expr = *p_cont_args_expr;
        const tuple& cont_args = boost::get<const tuple&>(cont_args_expr);
        bool need_container = true;
        for(auto i = cont_args.begin(); i != cont_args.end(); i++) {
            assert(detail::isinstance<literal>(*i));
            
            const literal& i_name = detail::up_get<const literal&>(*i);
            bool container_defined = m_decl_containers.exists(i_name.id());
            need_container = need_container && container_defined;
        }
        if (!need_container) {
            return n.ptr();
        }
        

        
        assert(detail::isinstance<name>(n.lhs()));
        const name& lhs = boost::get<const name&>(n.lhs());
        shared_ptr<const name> new_lhs =
            make_shared<const name>(
                detail::wrap_array_id(
                    lhs.id()),
                lhs.type().ptr(),
                cont_type);
        shared_ptr<const apply> new_rhs =
            make_shared<const apply>(
                apply_fn.ptr(),
                cont_args.ptr());

        //Add new container to declared containers
        m_decl_containers.insert(new_lhs->id());
        
        return make_shared<const suite>(
            make_vector<shared_ptr<const statement> >(n.ptr())
            (make_shared<const bind>(
                new_lhs,
                new_rhs)));
    }
    return n.ptr();
}

}
