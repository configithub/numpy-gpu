#include "tuple_break.hpp"

using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::string;
using std::move;
using backend::utility::make_vector;


namespace backend {

tuple_break::tuple_break()
    : m_supply("tuple") {}

tuple_break::result_type tuple_break::operator()(const bind& n) {
    bool lhs_tuple = detail::isinstance<tuple>(n.lhs());
    bool rhs_tuple = detail::isinstance<tuple>(n.rhs());
    if (lhs_tuple && rhs_tuple) {
        //tuple = tuple bind
        const tuple& lhs = boost::get<const tuple&>(n.lhs());
        const tuple& rhs = boost::get<const tuple&>(n.rhs());
        
        //This code assumes nested tuples have been flattened by
        //the expression flattener.
        //Because of this assumption, we expect the length of
        //both tuples in a tuple = tuple bind to be identical.
        //Assert to ensure this condition is not violated.
        assert(lhs.arity() == rhs.arity());
        vector<shared_ptr<const statement> > stmts;
        for(auto i = lhs.begin(), j = rhs.begin();
            i != lhs.end();
            i++, j++) {
            stmts.push_back(
                make_shared<bind>(
                    i->ptr(),
                    j->ptr()));
        }
        return make_shared<suite>(std::move(stmts));
    } else if (lhs_tuple && !rhs_tuple) {
        //Unpacking a tuple
        const tuple& lhs = boost::get<const tuple&>(n.lhs());
        vector<shared_ptr<const statement> > stmts;
        shared_ptr<const expression> p_rhs;
        if (detail::isinstance<apply>(n.rhs())) {
            //The RHS is a function call.  Call it, store the result,
            //then break it.  Here we call it and store the result.
            shared_ptr<const name> result = make_shared<const name>(
                m_supply.next(),
                lhs.type().ptr(),
                lhs.ctype().ptr());
            shared_ptr<const bind> call_stmt = make_shared<const bind>(result, 
                                                                       n.rhs().ptr());
            stmts.push_back(call_stmt);
            p_rhs = result;
        } else {
            //The RHS is not a function call, just use it directly
            p_rhs = n.rhs().ptr();
        }
        int number = 0;
        for(auto i = lhs.begin(); i != lhs.end(); i++, number++) {
            stmts.push_back(
                make_shared<const bind>(
                    i->ptr(),
                    make_shared<const apply>(
                        make_shared<const name>(
                            detail::snippet_get(number)),
                        make_shared<const tuple>(
                            make_vector<shared_ptr<const expression> >(p_rhs)))));
        }
        return make_shared<suite>(std::move(stmts));
    } else if (!lhs_tuple && rhs_tuple) {
        //Packing a tuple
        const tuple& rhs = boost::get<const tuple&>(n.rhs());
        
        vector<shared_ptr<const expression> > args;
        for(auto i = rhs.begin(); i != rhs.end(); i++) {
            args.push_back(i->ptr());
        }
        return make_shared<bind>(
            n.lhs().ptr(),
            make_shared<apply>(
                make_shared<name>(detail::snippet_make_tuple()),
                make_shared<tuple>(std::move(args))));
    } else {
        //No tuples in this bind, just return the original
        return n.ptr();
    }
}

tuple_break::result_type tuple_break::operator()(const procedure& n) {
    vector<shared_ptr<const expression> > args;
    vector<shared_ptr<const statement> > stmts;
    for(auto i = n.args().begin();
        i != n.args().end();
        i++) {
        //We can only allow names as arguments of a function
        assert(detail::isinstance<name>(*i));
        args.push_back(i->ptr());
    }
    if (stmts.size() == 0) {
        return this->rewriter::operator()(n);
    } else {
        shared_ptr<const suite> rewritten_stmts =
            static_pointer_cast<const suite>(
                this->rewriter::operator()(n.stmts()));
        for(auto i = rewritten_stmts->begin();
            i != rewritten_stmts->end();
            i++) {
            stmts.push_back(i->ptr());
        }
        return make_shared<const procedure>(
            n.id().ptr(),
            make_shared<backend::tuple>(std::move(args)),
            make_shared<backend::suite>(std::move(stmts)),
            n.type().ptr(),
            n.ctype().ptr(),
            n.place());
    }
    
}

}
