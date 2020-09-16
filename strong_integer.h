#pragma once

#include <atomic>
#include <limits>
#include <type_traits>

#include <boost/container_hash/hash.hpp>
#include <boost/preprocessor/cat.hpp>

#define PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(op)                                          \
    friend constexpr auto operator op(Strong a, Strong b) noexcept->Strong {                       \
        return Strong(a.get() op b.get());                                                         \
    }

#define PGSLAVE_STRONG_INTEGER_DEFINE_SHIFT_OPERATOR(op)                                           \
    friend constexpr auto operator op(Strong a, Strong b) noexcept->Strong {                       \
        return Strong(a.get() op b.get());                                                         \
    }

#define PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(op)                               \
    friend constexpr auto operator op(Strong a, Strong b) noexcept->bool {                         \
        return {a.get() op b.get()};                                                               \
    }

#define PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(op)                             \
    constexpr auto operator BOOST_PP_CAT(op, =)(Strong i) noexcept->Strong & {                     \
        *this = Strong(get() op i.get());                                                          \
        return *this;                                                                              \
    }                                                                                              \
    constexpr auto operator BOOST_PP_CAT(op, =)(Int i) noexcept->Strong & {                        \
        *this = Strong(get() op i);                                                                \
        return *this;                                                                              \
    }

#define PGSLAVE_STRONG_INTEGER_DEFINE_SHIFT_ASSIGNMENT_OPERATOR(op)                                \
    constexpr auto operator BOOST_PP_CAT(op, =)(Strong i) noexcept->Strong & {                     \
        *this = Strong(get() op i.get());                                                          \
        return *this;                                                                              \
    }                                                                                              \
    constexpr auto operator BOOST_PP_CAT(op, =)(int i) noexcept->Strong & {                        \
        *this = Strong(get() op i);                                                                \
        return *this;                                                                              \
    }

namespace pgslave::util {
struct StrongBase {};

template <typename T> struct is_strong_integer {
    static inline constexpr bool value = std::is_base_of_v<StrongBase, T>;
};

template <typename T>
static inline constexpr bool is_strong_integer_v = is_strong_integer<T>::value;

template <typename IntT, typename TagT> class Strong : public StrongBase {
  public:
    using Int = IntT;
    using Tag = TagT;
    static inline constexpr bool IsStrong = true;

    constexpr Strong() noexcept = default;
    /* explicit */ constexpr Strong(Int val) noexcept : m_val(val) {} // NOLINT

    /* explicit */ constexpr operator Int() const noexcept { return m_val; } // NOLINT

    [[nodiscard]] constexpr auto get() const noexcept { return m_val; }

    [[nodiscard]] constexpr auto operator&() const noexcept { return &m_val; }
    [[nodiscard]] constexpr auto operator&() noexcept { return &m_val; }

    constexpr auto operator++() noexcept -> Strong & { return (*this) += 1; }
    constexpr auto operator++(int) noexcept -> Strong {
        auto copy = *this;

        (*this) += 1;
        return copy;
    }

    constexpr auto operator--() noexcept -> Strong & { return (*this) += 1; }
    constexpr auto operator--(int) noexcept -> Strong {
        auto copy = *this;

        (*this) -= 1;
        return copy;
    }

    constexpr auto operator~() const noexcept { return Strong(~m_val); }
    constexpr auto operator!() const noexcept { return !m_val; }

    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(==);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(!=);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(<);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(>);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(>=);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(<=);

    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(+);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(-);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(*);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(/);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(%);

    PGSLAVE_STRONG_INTEGER_DEFINE_SHIFT_OPERATOR(<<);
    PGSLAVE_STRONG_INTEGER_DEFINE_SHIFT_OPERATOR(>>);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(|);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(&);
    PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(^);

    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(+);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(-);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(*);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(/);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(%);

    PGSLAVE_STRONG_INTEGER_DEFINE_SHIFT_ASSIGNMENT_OPERATOR(<<);
    PGSLAVE_STRONG_INTEGER_DEFINE_SHIFT_ASSIGNMENT_OPERATOR(>>);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(|);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(&);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(^);

  private:
    Int m_val = 0;
};

// Test for trivial copy and standard layout
static_assert(std::is_trivially_copyable_v<Strong<int, struct StrongLayoutTestTag>> &&
              std::is_standard_layout_v<Strong<int, struct StrongLayoutTestTag>>);

template <typename Strong, typename = std::enable_if_t<is_strong_integer_v<Strong>>>
constexpr auto hash_value(Strong i) -> std::size_t {
    std::size_t hash = 0;
    boost::hash_combine(hash, i.get());
    return hash;
}
} // namespace pgslave::util

namespace std {
template <typename Int, typename Tag> struct numeric_limits<pgslave::util::Strong<Int, Tag>> {
    static constexpr auto is_specialized = true;
    static constexpr auto is_signed = std::numeric_limits<Int>::is_signed;
    static constexpr auto is_integer = std::numeric_limits<Int>::is_integer;
    static constexpr auto is_exact = std::numeric_limits<Int>::is_exact;
    static constexpr auto has_infinity = std::numeric_limits<Int>::has_infinity;
    static constexpr auto has_quiet_NaN = std::numeric_limits<Int>::has_quiet_NaN;
    static constexpr auto has_signaling_NaN = std::numeric_limits<Int>::has_signaling_NaN;
    static constexpr auto has_denorm = std::numeric_limits<Int>::has_denorm;
    static constexpr auto has_denorm_loss = std::numeric_limits<Int>::has_denorm_loss;
    static constexpr auto round_style = std::numeric_limits<Int>::round_style;
    static constexpr auto is_iec559 = std::numeric_limits<Int>::is_iec559;
    static constexpr auto is_bounded = std::numeric_limits<Int>::is_bounded;
    static constexpr auto is_modulo = std::numeric_limits<Int>::is_modulo;
    static constexpr auto digits = std::numeric_limits<Int>::digits;
    static constexpr auto digits10 = std::numeric_limits<Int>::digits10;
    static constexpr auto max_digits10 = std::numeric_limits<Int>::max_digits10;
    static constexpr auto radix = std::numeric_limits<Int>::radix;
    static constexpr auto min_exponent = std::numeric_limits<Int>::min_exponent;
    static constexpr auto min_exponent10 = std::numeric_limits<Int>::min_exponent10;
    static constexpr auto max_exponent = std::numeric_limits<Int>::max_exponent;
    static constexpr auto max_exponent10 = std::numeric_limits<Int>::max_exponent10;
    static constexpr auto traps = std::numeric_limits<Int>::traps;
    static constexpr auto tinyness_before = std::numeric_limits<Int>::tinyness_before;

    static constexpr auto min() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::min());
    }
    static constexpr auto lowest() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::lowest());
    }
    static constexpr auto max() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::max());
    }
    static constexpr auto epsilon() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::epsilon());
    }
    static constexpr auto round_error() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::round_error());
    }
    static constexpr auto infinity() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::infinity());
    }
    static constexpr auto quiet_NaN() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::quiet_NaN());
    }
    static constexpr auto signaling_NaN() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::signaling_NaN());
    }
    static constexpr auto denorm_min() {
        return pgslave::util::Strong<Int, Tag>(std::numeric_limits<Int>::denorm_min());
    }
};

template <typename Int, typename Tag> struct hash<pgslave::util::Strong<Int, Tag>> {
  private:
    using base_type = pgslave::util::Strong<Int, Tag>;

  public:
    constexpr auto operator()(base_type i) const -> std::size_t {
        return boost::hash<base_type>{}(i);
    }
};
} // namespace std
