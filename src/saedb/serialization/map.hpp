#ifndef SAE_MAPSERIALIZE_HPP
#define SAE_MAPSERIALIZE_HPP

#include "iterator.hpp"
#include <iterator>
#include <unordered_map>

namespace sae{
namespace serialization{

namespace custom_serialization_impl {
    template <typename K, typename M>
    struct serialize_impl<OSerializeStream, std::unordered_map<K, M> > {
        static void run(OSerializeStream& ostr, std::unordered_map<K, M>& s) {
            auto begin = s.begin();
            auto end = s.end();
            ostr << s.size();

            for(;begin!=end;begin++) {
                ostr << (*begin).first << (*begin).second;
            }
        }
    };

    template <typename K, typename M>
    struct deserialize_impl<ISerializeStream, std::unordered_map<K, M> > {
        static void run(ISerializeStream& istr, std::unordered_map<K, M>& s) {
			//istr << (*((std::unorderemap<sdfsdf>*)&s))
			//	operator<< <ISe, std::unorder>(istr, s)
            s.clear();
            size_t len;
            istr >> len;
            for(size_t i = 0; i < len; ++i) {
                K k;
                M m;
                istr >> k >> m;
                s.insert(std::pair<K, M>(k,m));
            }
        }
    };
};

}}
#endif
