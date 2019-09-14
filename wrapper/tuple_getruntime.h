#include <stdexcept>
#include <stdlib.h>
#include <tuple>
#include <utility>

#define NOINLINE __attribute__((noinline))
#define ALWAYS_INLINE __attribute__((always_inline))

// return wrapper
template <class T, class U> struct return_ {
    static T &
    perform(U &u) NOINLINE {
        throw std::runtime_error("return type mismatch");
    }
};
template <class T> struct return_<T, T> {
    static T &
    perform(T &t) ALWAYS_INLINE {
        return t;
    }
};

template <class Ret, class Tuple, unsigned S> struct get_by_idx_ {
    static Ret &
    perform(Tuple &t, int idx) ALWAYS_INLINE {
        constexpr unsigned C = S - 1;
        if(idx == C) {
            typedef typename std::tuple_element<C, Tuple>::type elem_type;
            return return_<Ret, elem_type>::perform(std::get<C>(t));
        } else {
            return get_by_idx_<Ret, Tuple, C>::perform(t, idx);
        }
    }
};
template <class Ret, class Tuple> struct get_by_idx_<Ret, Tuple, 0> {
    static Ret &
    perform(Tuple &, int) NOINLINE {
        throw std::runtime_error("bad field index");
    }
};

template <class Ret, class Tuple>
Ret &
get(Tuple &t, int idx) {
    constexpr unsigned S = std::tuple_size<Tuple>::value;
    return get_by_idx_<Ret, Tuple, S>::perform(t, idx);
}