// Copyright © 2025 Giorgio Audrito. All Rights Reserved.

/**
 * @file placed.hpp
 * @brief Implementation and helper functions for the `placed<tier,T,p,q>` class template for placed neighboring fields.
 */

#ifndef FCPP_DATA_PLACED_H_
#define FCPP_DATA_PLACED_H_

#include "lib/common/option.hpp"
#include "lib/data/field.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {


//! @brief Compile-time function checking whether a bitmask is a subset of another bitmask.
template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr bool bitsubset(T x, T y) {
    return not bool(x & ~y);
}

//! @brief Compile-time bitwise intersection of multiple tier bitmasks (base case).
template <tier_t... xs>
constexpr tier_t tier_inf = (tier_t)-1;

//! @brief Compile-time bitwise intersection of multiple tier bitmasks (general case).
template <tier_t x, tier_t... xs>
constexpr tier_t tier_inf<x, xs...> = x & tier_inf<xs...>;

//! @brief Compile-time bitwise union of multiple tier bitmasks (base case).
template <tier_t... xs>
constexpr tier_t tier_sup = (tier_t)0;

//! @brief Compile-time bitwise union of multiple tier bitmasks (general case).
template <tier_t x, tier_t... xs>
constexpr tier_t tier_sup<x, xs...> = x | tier_sup<xs...>;


//! @cond INTERNAL
//! @brief Forward declarations
//! @{
template <tier_t tier, typename T, tier_t p = tier_t(-1), tier_t q = 0> class placed;

namespace details {
    template <tier_t tier, typename T, tier_t p, tier_t q, typename F>
    placed<tier,T,p,q> place_data(common::type_sequence<placed<tier,T,p,q>>, F&&);
    template <tier_t tier, typename T, tier_t p, tier_t q>
    auto const& maybe_get_data(placed<tier,T,p,q> const&);
}
//! @}
//! @endcond


/**
 * @name extract_tier
 *
 * Constant which extracts the tier information from a sequence of types,
 * provided that at least one of them is placed or a placed tuple.
 * If no argument is placed, it returns zero (invalid empty tier).
 */
//! @{
//! @brief Invalid case.
template <class... Ts>
constexpr tier_t extract_tier = 0;

//! @brief Ignore constness.
template <class A, class... Ts>
constexpr tier_t extract_tier<A const, Ts...> = extract_tier<A, Ts...>;

//! @brief Ignore lvalue references.
template <class A, class... Ts>
constexpr tier_t extract_tier<A&, Ts...> = extract_tier<A, Ts...>;

//! @brief Ignore rvalue references.
template <class A, class... Ts>
constexpr tier_t extract_tier<A&&, Ts...> = extract_tier<A, Ts...>;

//! @brief Ignoring non-placed types.
template <class A, class... Ts>
constexpr tier_t extract_tier<A, Ts...> = extract_tier<Ts...>;

//! @brief Extract from placed type.
template <tier_t tier, class A, tier_t p, tier_t q, class... Ts>
constexpr tier_t extract_tier<placed<tier,A,p,q>, Ts...> = tier;

//! @brief Expand tuple-like type.
template <template<class...> class U, class... As, class... Ts>
constexpr tier_t extract_tier<U<As...>, Ts...> = extract_tier<As..., Ts...>;

//! @brief Expand array-like type.
template <template<class,size_t> class U, class A, size_t N, class... Ts>
constexpr tier_t extract_tier<U<A, N>, Ts...> = extract_tier<A, Ts...>;
//! @}


/**
 * @name is_placed
 *
 * Constant which is true if and only if at least one of the type parameters are built through array-like
 * and tuple-like classes from specializations of the placed template.
 */
template <class... Ts>
constexpr bool is_placed = extract_tier<Ts...> != 0;


//! @cond INTERNAL
namespace details {
    //! @brief General form.
    template <tier_t tier, typename A, bool b = is_placed<A> or common::has_template<field, A>>
    struct to_placed;

    //! @brief Base case assuming no occurrences of the template.
    template <tier_t tier, class A>
    struct to_placed<tier, A, false> {
        using value_type = common::partial_decay<A>;
        static constexpr tier_t p_value = tier_t(-1);
        static constexpr tier_t q_value = 0;
        using type = placed<tier, std::decay_t<A>>;
    };

    //! @brief Mixing multiple placed types.
    template <tier_t tier, template<class...> class U, class... A>
    struct mix_placed {
        using value_type = U<typename A::value_type...>;
        static constexpr tier_t p_value = tier_inf<A::p_value...>;
        static constexpr tier_t q_value = tier_sup<A::q_value...>;
        using type = placed<tier, value_type, p_value, q_value>;
    };

    //! @brief Extracts occurrences from the arguments of a tuple-like type.
    template <tier_t tier, template<class...> class U, class... A>
    struct to_placed<tier, U<A...>, true> : public mix_placed<tier, U, to_placed<tier, common::partial_decay<A>>...> {};

    //! @brief Extracts occurrences from the arguments of a tuple&-like type.
    template <tier_t tier, template<class...> class U, class... A>
    struct to_placed<tier, U<A...>&, true> : public to_placed<tier, U<A&...>, true> {};

    //! @brief Extracts occurrences from the arguments of a tuple const&-like type.
    template <tier_t tier, template<class...> class U, class... A>
    struct to_placed<tier, U<A...> const&, true> : public to_placed<tier, U<A const&...>, true> {};

    //! @brief Extracts occurrences from the argument of an array-like type.
    template <tier_t tier, template<class,size_t> class U, class A, size_t N>
    struct to_placed<tier, U<A, N>, true> {
        using subtype = to_placed<tier, common::partial_decay<A>, true>;
        using value_type = U<std::decay_t<typename subtype::value_type>, N>;
        static constexpr tier_t p_value = subtype::p_value;
        static constexpr tier_t q_value = subtype::q_value;
        using type = placed<tier, value_type, p_value, q_value>;
    };

    //! @brief Propagate & assuming occurrences of the template are present.
    template <tier_t tier, template<class,size_t> class U, class A, size_t N>
    struct to_placed<tier, U<A, N>&, true> : public to_placed<tier, U<std::decay_t<A>, N>, true> {};

    //! @brief Propagate const& assuming occurrences of the template are present.
    template <tier_t tier, template<class,size_t> class U, class A, size_t N>
    struct to_placed<tier, U<A, N> const&, true> : public to_placed<tier, U<std::decay_t<A>, N>, true> {};

    //! @brief For a single placed<...> argument.
    template <tier_t tier, tier_t t, typename T, tier_t p, tier_t q>
    struct single_placed {
        static_assert(tier == t, "mixing up different tiers: cannot appy to_placed<tier, T> to a T with a different tier");
        using value_type = typename common::details::vectorize<common::partial_decay<T>>::type;
        static constexpr tier_t p_value = p;
        static constexpr tier_t q_value = q;
        using type = placed<tier, std::decay_t<T>, p, q>;
    };

    //! @brief If the second parameter is of the form placed<...>.
    template <tier_t tier, tier_t t, typename T, tier_t p, tier_t q>
    struct to_placed<tier, placed<t,T,p,q>, true> : public single_placed<tier, t, T, p, q> {};

    //! @brief If the second parameter is of the form placed<...>&.
    template <tier_t tier, tier_t t, typename T, tier_t p, tier_t q>
    struct to_placed<tier, placed<t,T,p,q>&, true> : public single_placed<tier, t, T&, p, q> {};

    //! @brief If the second parameter is of the form placed<...> const&.
    template <tier_t tier, tier_t t, typename T, tier_t p, tier_t q>
    struct to_placed<tier, placed<t,T,p,q> const&, true> : public single_placed<tier, t, T const&, p, q> {};

    //! @brief If the second parameter is of the form field<...>.
    template <tier_t tier, typename T>
    struct to_placed<tier, field<T>, true> : public single_placed<tier, tier, T, tier_t(-1), tier_t(-1)> {};

    //! @brief If the second parameter is of the form field<...>&.
    template <tier_t tier, typename T>
    struct to_placed<tier, field<T>&, true> : public single_placed<tier, tier, T&, tier_t(-1), tier_t(-1)> {};

    //! @brief If the second parameter is of the form field<...> const&.
    template <tier_t tier, typename T>
    struct to_placed<tier, field<T> const&, true> : public single_placed<tier, tier, T const&, tier_t(-1), tier_t(-1)> {};
}
//! @endcond INTERNAL

//! @brief Converts a type to a placed field.
template <tier_t tier, typename A>
using to_placed = details::to_placed<tier, common::partial_decay<A>>;


//! @brief Extract the non-placed type from a placed type.
template <tier_t tier, typename A>
using del_placed = typename details::to_placed<tier, A>::value_type;


/**
 * @brief Class representing a placed neighboring field of T values.
 */
template <tier_t tier, typename T, tier_t p, tier_t q>
class placed {
    // TODO: base class for conversion to bool & conversion from field if compatible with parameters
    static_assert(tier != 0 and (tier & (tier-1)) == 0, "tier must be atomic (with a single bit set)");
    static_assert(not common::has_template<field, T>, "cannot instantiate a placed field of fields");
    static_assert(not is_placed<T>, "cannot instantiate a placed field of placed fields");

    //! @brief Checks whether the current type is compatible with another placed type.
    template <typename A, tier_t p1, tier_t q1>
    static constexpr bool is_compatible = std::is_convertible<A,T>::value and bitsubset(p, p1) and bitsubset(q1, q);

    //! @cond INTERNAL
    //! @brief Class friendships
    //! @{
    template <tier_t t1, typename A, tier_t p1, tier_t q1>
    friend class placed;
    //! @}

    //! @brief Function friendships
    //! @{
    template <tier_t tier1, typename T1, tier_t p1, tier_t q1, typename F>
    friend placed<tier1,T1,p1,q1> details::place_data(common::type_sequence<placed<tier1,T1,p1,q1>>, F&&);
    friend auto const& details::maybe_get_data(placed<tier,T,p,q> const&);
    //! @}
    //! @endcond

  public:
    //! @brief The contained type.
    using value_type = T;
    //! @brief The underlying field type.
    using field_type = std::conditional_t<bool(q), field<T>, T>;
    //! @brief The overal optional type.
    using option_type = common::option<field_type, bool(tier & p)>;
    //! @brief The atomic tier of the device currently running the program.
    static constexpr tier_t tier_value = tier;
    //! @brief The tier class of the devices where the data exists.
    static constexpr tier_t p_value = p;
    //! @brief The tier class of the devices from which neighbouring data comes from.
    static constexpr tier_t q_value = q;

    //! @name constructors
    //! @{

    //! @brief Default constructor (dangerous: if q>0 creates a placed field in an invalid state).
    placed() = default;

    //! @brief Constant placed field (copying).
    placed(T const& d) : m_data(d) {}

    //! @brief Constant placed field (moving).
    placed(T&& d) : m_data(std::move(d)) {}

    //! @brief Copy constructor.
    placed(placed const&) = default;

    //! @brief Move constructor.
    placed(placed&&) = default;

    //! @brief Implicit conversion copy constructor.
    template <typename A, tier_t p1, tier_t q1, typename = std::enable_if_t<is_compatible<A,p1,q1>>>
    placed(placed<tier, A, p1, q1> const& f) : m_data(f.m_data) {}

    //! @brief Implicit conversion move constructor.
    template <typename A, tier_t p1, tier_t q1, typename = std::enable_if_t<is_compatible<A,p1,q1>>>
    placed(placed<tier, A, p1, q1>&& f) : m_data(std::move(f.m_data)) {}
    //! @}

    //! @name assignment operators
    //! @{

    //! @brief Copy assignment.
    placed& operator=(placed const&) = default;

    //! @brief Move assignment.
    placed& operator=(placed&&) = default;

    //! @brief Implicit conversion copy assignment.
    template <typename A, tier_t p1, tier_t q1, typename = std::enable_if_t<is_compatible<A,p1,q1>>>
    placed& operator=(placed<tier, A, p1, q1> const& f) {
        m_data = f.m_data;
        return *this;
    }

    //! @brief Implicit conversion move assignment.
    template <typename A, tier_t p1, tier_t q1, typename = std::enable_if_t<is_compatible<A,p1,q1>>>
    placed& operator=(placed<tier, A, p1, q1>&& f) {
        m_data = std::move(f.m_data);
        return *this;
    }
    //! @}

    //! @brief Write access to the underlying field (fails if p is not `tier_t(-1)`).
    field_type& get() {
        return get_impl(std::integral_constant<bool, p == tier_t(-1)>{});
    }

    //! @brief Const access to the underlying field (fails if p is not `tier_t(-1)`).
    field_type const& get() const {
        return get_impl(std::integral_constant<bool, p == tier_t(-1)>{});
    }

    //! @brief Const access to the underlying field, with a default if no value available.
    field_type get_or(field_type f) const {
        return get_or_impl(std::integral_constant<bool, bool(tier & p)>{}, f);
    }

    //! @brief Exchanges the content of the `placed` objects.
    void swap(placed& f) {
        swap(m_data, f.m_data);
    }

    //! @brief Serialises the content from a given input stream.
    common::isstream& serialize(common::isstream& s) {
        return s & m_data;
    }

    //! @brief Serialises the content to a given output stream.
    template <typename S>
    S& serialize(S& s) const {
        return s & m_data;
    }

  private:
    //! @brief Field constructor (copying).
    placed(field<T> const& f) : m_data(maybe_field(std::integral_constant<int, (tier&p) and q>{}, f)) {}

    //! @brief Field constructor (moving).
    placed(field<T>&& f) : m_data(maybe_field(std::integral_constant<bool, (tier&p) and q>{}, std::move(f))) {}

    //! @brief Field constructor argument forwarding (active).
    template <typename F>
    F&& maybe_field(std::true_type, F&& f) {
        return std::forward<F>(f);
    }

    //! @brief Field constructor argument forwarding (inactive).
    template <typename F>
    option_type maybe_field(std::false_type, F&&) {
        return {};
    }

    //! @brief Write access to the underlying field provided that p is `tier_t(-1)`.
    field_type& get_impl(std::true_type) {
        return m_data.front();
    }

    //! @brief Const access to the underlying field provided that p is `tier_t(-1)`.
    field_type const& get_impl(std::true_type) const {
        return m_data.front();
    }

    //! @brief Const access to the underlying field with a default (value available).
    field_type get_or_impl(std::true_type, field_type f) const {
        return m_data.front();
    }
    //! @brief Const access to the underlying field with a default (no value available).
    field_type get_or_impl(std::false_type, field_type f) const {
        return f;
    }

    //! @brief Ordered IDs of exceptions.
    option_type m_data;
};


//! @cond INTERNAL
namespace details {
    //! @brief Accesses private constructors for placed.
    template <tier_t tier, typename T, tier_t p, tier_t q, typename F>
    placed<tier,T,p,q> place_data(common::type_sequence<placed<tier,T,p,q>>, F&& f) {
        return std::forward<F>(f);
    }

    //! @brief Builds a placed field from member values.
    template <tier_t tier, typename T, tier_t p, tier_t q>
    inline placed<tier,T,p,q> make_placed(std::vector<device_t>&& ids, std::vector<T>&& vals) {
        return place_data(common::type_sequence<placed<tier,T,p,q>>{}, make_field(std::move(ids), std::move(vals)));
    }

    //! @brief Accesses inner data of a possibly placed field (empty overload).
    template <typename T>
    T const& maybe_get_data(T const& x) {
        return x;
    }
    //! @brief Accesses inner data of a possibly placed field (active overload).
    template <tier_t tier, typename T, tier_t p, tier_t q>
    auto const& maybe_get_data(placed<tier,T,p,q> const& x) {
        return x.m_data.front();
    }

    //! @brief Applies an operator pointwise on a sequence of placed fields (empty overload).
    template <typename T, typename... Ts>
    T maybe_map_hood(std::false_type, common::type_sequence<T>, Ts&&...) {
        return {};
    }
    //! @brief Applies an operator pointwise on a sequence of placed fields (active overload).
    template <typename T, typename F, typename... Ts>
    T maybe_map_hood(std::true_type, common::type_sequence<T> t, F&& op, Ts const&... xs) {
        return place_data(t, map_hood(op, maybe_get_data(xs)...));
    }

    //! @brief Applies an operator pointwise on a sequence of placed fields (not placed overload).
    template <typename P, typename... Ss, typename F, typename... Ts>
    auto pmap_hood(common::number_sequence<0>, P, common::type_sequence<tuple<Ss...>>, F&& op, Ts const&... xs) {
        return map_hood(op, xs...);
    }
    //! @brief Applies an operator pointwise on a sequence of placed fields (placed overload).
    template <intmax_t tier, typename P, typename... Ss, typename F, typename... Ts>
    auto pmap_hood(common::number_sequence<tier>, P, common::type_sequence<tuple<Ss...>>, F&& op, Ts const&... xs) {
        constexpr tier_t p = P::p_value;
        constexpr tier_t q = P::q_value;
        using T = local_result<F, Ss...>;
        return maybe_map_hood(std::integral_constant<bool, bool(tier & p)>{}, common::type_sequence<placed<tier,T,p,q>>{}, std::forward<F>(op), xs...);
    }
}
//! @endcond

/**
 * @name pmap_hood
 *
 * Applies an operator pointwise on a sequence of placed fields.
 */
template <typename F, typename... Ts>
auto pmap_hood(F&& op, Ts const&... xs) {
    constexpr tier_t tier = extract_tier<Ts...>;
    using P = to_placed<tier, tuple<Ts const&...>>;
    return details::pmap_hood(common::number_sequence<tier>{}, P{}, common::type_sequence<typename P::value_type>{}, op, xs...);
}
// TODO: pmap_hood for tuples of placed
// TODO: rename to map_hood while avoiding conflicts with that in field.hpp


//! @cond INTERNAL
namespace details {
    //! @brief The result type of a placed fold operation.
    template <tier_t tier, typename F, typename A, typename B, tier_t p>
    using fold_result = placed<tier, std::result_of_t<F(A const&, B const&)>, p, 0>;

    //! @brief Inclusive folding (inactive).
    template <typename F, tier_t tier, typename A, tier_t p, tier_t q>
    inline fold_result<tier, F, A, A, p>
    maybe_fold_hood(std::false_type, F&& op, placed<tier,A,p,q> const& f, std::vector<device_t> const& dom) {
        return {};
    }

    //! @brief Inclusive folding (active).
    template <typename F, tier_t tier, typename A, tier_t p, tier_t q>
    inline fold_result<tier, F, A, A, p>
    maybe_fold_hood(std::true_type, F&& op, placed<tier,A,p,q> const& f, std::vector<device_t> const& dom) {
        return fold_hood(std::forward<F>(op), maybe_get_data(f), dom);
    }

    //! @brief Exclusive folding (inactive).
    template <typename F, tier_t tier, typename A, tier_t p, tier_t q, typename B>
    inline fold_result<tier, F, A, B, p>
    maybe_fold_hood(std::false_type, F&& op, placed<tier,A,p,q> const& f, B const& b, std::vector<device_t> const& dom, device_t i) {
        return {};
    }

    //! @brief Exclusive folding (active).
    template <typename F, tier_t tier, typename A, tier_t p, tier_t q, typename B>
    inline fold_result<tier, F, A, B, p>
    maybe_fold_hood(std::true_type, F&& op, placed<tier,A,p,q> const& f, B const& b, std::vector<device_t> const& dom, device_t i) {
        return fold_hood(std::forward<F>(op), maybe_get_data(f), b, dom, i);
    }

    /**
     * @name fold_hood
     *
     * Reduces the values in a part of a placed field (determined by domain) to a single value through a binary operation.
     */
    //! @{
    //! @brief Inclusive folding.
    template <typename F, tier_t tier, typename A, tier_t p, tier_t q>
    inline fold_result<tier, F, A, A, p>
    fold_hood(F&& op, placed<tier,A,p,q> const& f, std::vector<device_t> const& dom) {
        return maybe_fold_hood(std::integral_constant<bool, bool(tier&p)>{}, std::forward<F>(op), f, dom);
    }
    //! @brief Exclusive folding.
    template <typename F, tier_t tier, typename A, tier_t p, tier_t q, typename B>
    inline fold_result<tier, F, A, B, p>
    fold_hood(F&& op, placed<tier,A,p,q> const& f, B const& b, std::vector<device_t> const& dom, device_t i) {
        return maybe_fold_hood(std::integral_constant<bool, bool(tier&p)>{}, std::forward<F>(op), f, b, dom, i);
    }
    // TODO: fold for tuples of placed
    //! @}
}
//! @endcond


/**
 * @brief Overloads unary operators for placed fields.
 *
 * Used to overload every operator available for the base type.
 * Macro not available outside of the scope of this file.
 */
#define _DEF_UOP(op)                                                                         \
template <typename A, tier_t tier = extract_tier<A>, typename = std::enable_if_t<tier != 0>> \
auto operator op(A&& x) {                                                                    \
    return pmap_hood([](auto const& a){ return op a; }, std::forward<A>(x));                 \
}

/**
 * @brief Overloads binary operators for placed fields.
 *
 * Used to overload every operator available for the base type.
 * Macro not available outside of the scope of this file.
 */
#define _DEF_BOP(op)                                                                                              \
template <typename A, typename B, tier_t tier = extract_tier<A, B>, typename = std::enable_if_t<tier != 0>>       \
auto operator op(A&& x, B&& y) {                                                                                  \
    return pmap_hood([](auto const& a, auto const& b){ return a op b; }, std::forward<A>(x), std::forward<B>(y)); \
}

///**
// * @brief Overloads composite assignment operators for fields.
// *
// * Used to overload every operator available for the base type.
// * Macro not available outside of the scope of this file.
// */
//#define _DEF_IOP(op)                                                                                    \
//template <typename A, typename B>                                                                       \
//field<A>& operator op##=(field<A>& x, B const& y) {                                                     \
//    return pmod_hood([](A const& a, to_local<B> const& b) { return std::move(a) op b; }, x, y);          \
//}


_DEF_UOP(+)
_DEF_UOP(-)
_DEF_UOP(~)
_DEF_UOP(!)

_DEF_BOP(+)
_DEF_BOP(-)
_DEF_BOP(*)
_DEF_BOP(/)
_DEF_BOP(%)
_DEF_BOP(^)
_DEF_BOP(&)
_DEF_BOP(|)
_DEF_BOP(<)
_DEF_BOP(>)
_DEF_BOP(<=)
_DEF_BOP(>=)
_DEF_BOP(==)
_DEF_BOP(!=)
_DEF_BOP(&&)
_DEF_BOP(||)

//_DEF_IOP(+)
//_DEF_IOP(-)
//_DEF_IOP(*)
//_DEF_IOP(/)
//_DEF_IOP(%)
//_DEF_IOP(^)
//_DEF_IOP(&)
//_DEF_IOP(|)
//_DEF_IOP(>>)
//_DEF_IOP(<<)


#undef _DEF_UOP
#undef _DEF_BOP
#undef _DEF_IOP


}

#endif // FCPP_DATA_PLACED_H_
