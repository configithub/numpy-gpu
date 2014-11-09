#include "type_printer.hpp"

namespace backend
{

repr_type_printer::repr_type_printer(std::ostream &os)
    : m_os(os) {}

void repr_type_printer::operator()(const monotype_t &mt) {
    m_os << mt.name();
    if(mt.begin() != mt.end()) {
        open();
        for(auto i = mt.begin();
            i != mt.end();
            i++) {
            boost::apply_visitor(*this, *i);
            if (std::next(i) != mt.end()) 
                sep();
        }
        close();
    }
}
void repr_type_printer::operator()(const polytype_t &pt) {
    m_os << "Polytype(";
    for(auto i = pt.begin();
        i != pt.end();
        i++) {
        boost::apply_visitor(*this, *i);
        m_os << ", ";
    }
    boost::apply_visitor(*this, pt.monotype());
    m_os << ")";
}
void repr_type_printer::sep() const {
    m_os << ", ";
}
void repr_type_printer::open() const {
    m_os << "(";
}
void repr_type_printer::close() const {
    m_os << ")";
}

namespace ctype {
ctype_printer::ctype_printer(
    const copperhead::system_variant& t,
    std::ostream &os)
    : m_t(t), m_os(os)
{
    m_need_space.push(false);
}
void ctype_printer::operator()(const monotype_t &mt) {
    m_os << mt.name();
    m_need_space.top() = (mt.name()[mt.name().size()-1] == '>');
}
void ctype_printer::operator()(const sequence_t &st) {
    m_os << st.name() << "<";
    m_os << copperhead::to_string(m_t) << ", ";
    boost::apply_visitor(*this, st.sub());
    m_os << ">";
    m_need_space.top() = true;
}

void ctype_printer::operator()(const zipped_sequence_t &zt) {
    m_os << zt.name() << "<";
    m_need_space.push(false);
    boost::apply_visitor(*this, zt.sub());
    if (m_need_space.top())
        m_os << " ";
    m_need_space.pop();
    m_os << ">";
    m_need_space.top() = true;
    
}

void ctype_printer::operator()(const cuarray_t &ct) {
    //Because cuarray_t is a variant, we don't want to
    //print out the template definition.
    this->operator()((monotype_t)ct);
    m_need_space.top() = false;
}

void ctype_printer::operator()(const polytype_t &pt) {
    boost::apply_visitor(*this, pt.monotype());
    m_os << "<";
    m_need_space.push(false);
    for(auto i = pt.begin();
        i != pt.end();
        i++) {
        boost::apply_visitor(*this, *i);
        if (std::next(i) != pt.end()) {
            m_os << ", ";
        }
    }
    //This can be removed once C++OX support is nvcc
    //It is a workaround to prevent emitting foo<bar<baz>>
    //And instead emit foo<bar<baz> > 
    if (m_need_space.top())
        m_os << " ";
    m_need_space.pop();
    m_need_space.top() = true;
    m_os << ">";
}

void ctype_printer::operator()(const tuple_t& tt) {
    if (!tt.boost_impl()) {
        m_os << "thrust::tuple<";
    } else {
        m_os << "boost::tuple<";
    }
    m_need_space.push(false);
    for (auto i = tt.begin(); i != tt.end(); i++) {
        boost::apply_visitor(*this, *i);
        if (std::next(i) != tt.end()) {
            m_os << ", ";
        }
    }
    if (m_need_space.top())
        m_os << " ";
    m_need_space.pop();
    m_need_space.top() = true;
    m_os << ">";
}
        

void ctype_printer::sep() const {
    m_os << ", ";
}
void ctype_printer::open() const {
    m_os << "(";
}
void ctype_printer::close() const {
    m_os << ")";
}

}
}
