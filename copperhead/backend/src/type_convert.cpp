#include "type_convert.hpp"
#include "utility/isinstance.hpp"

using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::vector;
using std::move;

namespace backend {

namespace detail {

cu_to_c::result_type cu_to_c::operator()(const monotype_t& mt) {
    if (mt.name() == "Int32") {
        return ctype::int32_mt;
    } else if (mt.name() == "Int64") {
        return ctype::int64_mt;
    } else if (mt.name() == "Uint32") {
        return ctype::uint32_mt;
    } else if (mt.name() == "Uint64") {
        return ctype::uint64_mt;
    } else if (mt.name() == "Float32") {
        return ctype::float32_mt;
    } else if (mt.name() == "Float64") {
        return ctype::float64_mt;
    } else if (mt.name() == "Bool") {
        return ctype::bool_mt;
    } else if (mt.name() == "Void") {
        return ctype::void_mt;
    } else {
        return result_type(new ctype::monotype_t(mt.name()));
    }
}

struct sequenceize
    : boost::static_visitor<shared_ptr<const ctype::type_t> > {
    
    template<typename T>
    result_type operator()(const T& t) const {
        return make_shared<const ctype::sequence_t>(t.ptr());
    }

    result_type operator()(const ctype::tuple_t& t) const {
        vector<shared_ptr<const ctype::type_t> > subs;
        for(auto i = t.begin(); i != t.end(); i++) {
            subs.push_back(boost::apply_visitor(*this, *i));
        }
        return make_shared<const ctype::zipped_sequence_t>(
            make_shared<const ctype::tuple_t>(
                std::move(subs)));
    }
    
};

cu_to_c::result_type cu_to_c::operator()(const sequence_t & st) {
    result_type sub = boost::apply_visitor(*this, st.sub());
    //We represent all AoS types as SoA
    //This is the conversion:
    if (isinstance<ctype::tuple_t>(*sub)) {
        //Convert leaf subtypes to sequences
        return boost::apply_visitor(sequenceize(), *sub);
    }
    return result_type(new ctype::sequence_t(sub));
}
cu_to_c::result_type cu_to_c::operator()(const tuple_t& tt) {
    vector<result_type> subs;
    for(auto i = tt.begin(); i != tt.end(); i++) {
        subs.push_back(boost::apply_visitor(*this, *i));
    }
    return result_type(new ctype::tuple_t(std::move(subs)));
}
cu_to_c::result_type cu_to_c::operator()(const fn_t& ft) {
    shared_ptr<const ctype::tuple_t> args =
        static_pointer_cast<const ctype::tuple_t>(
            boost::apply_visitor(*this, ft.args()));
    shared_ptr<const ctype::type_t> result =
        boost::apply_visitor(*this, ft.result());
    result_type fn_result(new ctype::fn_t(args, result));
    return fn_result;
}

cu_to_c::result_type cu_to_c::operator()(const polytype_t& p) {
    vector<result_type> subs;
    for(auto i = p.begin();
        i != p.end();
        i++) {
        subs.push_back(boost::apply_visitor(*this, *i));
    }
    auto base = static_pointer_cast<const ctype::monotype_t>(boost::apply_visitor(*this, p.monotype()));
    return result_type(new ctype::polytype_t(std::move(subs), base));
}

}


type_convert::type_convert() : m_c() {}
type_convert::result_type type_convert::operator()(const procedure &p) {
    shared_ptr<const name> id =
        static_pointer_cast<const name>(this->operator()(p.id()));
    shared_ptr<const tuple> args =
        static_pointer_cast<const tuple>(this->operator()(p.args()));
    shared_ptr<const suite> stmts =
        static_pointer_cast<const suite>(this->operator()(p.stmts()));
    shared_ptr<const type_t> t = p.type().ptr();
        
    //Yes, I really want to make a ctype from a type. That's the point!
    shared_ptr<const ctype::type_t> ct = boost::apply_visitor(m_c, p.type());

    shared_ptr<const node> result(new procedure(id, args, stmts, t, ct));
    return result;
}
type_convert::result_type type_convert::operator()(const name &p) {
    shared_ptr<const type_t> t = p.type().ptr();
        
    //Yes, I really want to make a ctype from a type. That's the point!
    shared_ptr<const ctype::type_t> ct = boost::apply_visitor(m_c, p.type());
    result_type result(new name(p.id(), t, ct));
    return result;
}

type_convert::result_type type_convert::operator()(const literal &p) {
    shared_ptr<const type_t> t = p.type().ptr();
        
    //Yes, I really want to make a ctype from a type. That's the point!
    shared_ptr<const ctype::type_t> ct = boost::apply_visitor(m_c, p.type());
    result_type result(new literal(p.id(), t, ct));
    return result;
}


}
