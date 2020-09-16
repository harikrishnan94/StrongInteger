#pragma once

#include <atomic>
#include <limits>
#include <type_traits>

#include <boost/container_hash/hash.hpp>
#include <boost/preprocessor/cat.hpp>

#define ResType(Int1, Int2, Tag, op) Strong<decltype(Int1(0) op Int2(0)), Tag>

#define PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(op)                                          \
    template <typename Int1, typename Int2, typename Tag>                                          \
    constexpr auto operator op(Strong<Int1, Tag> a, Strong<Int2, Tag> b)                           \
        ->ResType(Int1, Int2, Tag, +) {                                                            \
        return {a.get() op b.get()};                                                               \
    }                                                                                              \
                                                                                                   \
    template <typename Int1, typename Int2, typename Tag1, typename Tag2>                          \
    constexpr auto operator op(Strong<Int1, Tag1> a, Strong<Int2, Tag2> b) = delete;               \
                                                                                                   \
    template <typename Int1, typename Int2, typename Tag,                                          \
              typename = std::enable_if_t<!is_strong_integer_v<Int2>>>                             \
    constexpr auto operator op(Strong<Int1, Tag> a, Int2 b) noexcept->ResType(Int1, Int2, Tag,     \
                                                                              op) {                \
        return {a.get() op b};                                                                     \
    }                                                                                              \
                                                                                                   \
    template <typename Int1, typename Int2, typename Tag,                                          \
              typename = std::enable_if_t<!is_strong_integer_v<Int1>>>                             \
    constexpr auto operator op(Int1 a, Strong<Int2, Tag> b) noexcept {                             \
        return a op b.get();                                                                       \
    }

#define PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(op)                               \
    template <typename Int1, typename Int2, typename Tag>                                          \
    constexpr auto operator op(Strong<Int1, Tag> a, Strong<Int2, Tag> b)->bool {                   \
        return a.get() op b.get();                                                                 \
    }                                                                                              \
                                                                                                   \
    template <typename Int1, typename Int2, typename Tag1, typename Tag2>                          \
    constexpr auto operator op(Strong<Int1, Tag1> a, Strong<Int2, Tag2> b) = delete;               \
                                                                                                   \
    template <typename Int1, typename Int2, typename Tag,                                          \
              typename = std::enable_if_t<!is_strong_integer_v<Int2>>>                             \
    constexpr auto operator op(Strong<Int1, Tag> a, Int2 b) noexcept->bool {                       \
        return a.get() op b;                                                                       \
    }                                                                                              \
                                                                                                   \
    template <typename Int1, typename Int2, typename Tag,                                          \
              typename = std::enable_if_t<!is_strong_integer_v<Int1>>>                             \
    constexpr auto operator op(Int1 a, Strong<Int2, Tag> b) noexcept->bool {                       \
        return a op b.get();                                                                       \
    }

#define PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(op)                             \
    template <typename Int2>                                                                       \
    constexpr auto operator BOOST_PP_CAT(op, =)(Strong<Int2, Tag> i) noexcept->Strong & {          \
        *this = Strong(get() op i.get());                                                          \
        return *this;                                                                              \
    }                                                                                              \
                                                                                                   \
    template <typename Int2, typename Tag2>                                                        \
    constexpr auto operator BOOST_PP_CAT(op, =)(Strong<Int2, Tag2> i) = delete;                    \
                                                                                                   \
    constexpr auto operator BOOST_PP_CAT(op, =)(Int i) noexcept->Strong & {                        \
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

template <typename IntT, typename TagT, typename Enable = void> class Strong;

template <typename IntT, typename TagT>
class Strong<IntT, TagT,
             typename std::enable_if_t<std::conjunction_v<
                 std::disjunction<std::is_integral<IntT>, std::is_floating_point<IntT>>,
                 std::negation<std::is_same<bool, IntT>>>>> : public StrongBase {
  public:
    using Int = IntT;
    using Tag = TagT;
    static inline constexpr bool IsStrong = true;

    constexpr Strong() noexcept = default;

    /* explicit */ constexpr Strong(Int val) noexcept : m_val(val) {} // NOLINT

    // Delete Construction from OtherTag types
    template <typename OtherInt, typename OtherTag,
              std::enable_if_t<!std::is_same_v<Tag, OtherTag>>>
    explicit Strong(Strong<OtherInt, OtherTag> val) = delete;

    // Enable Assignment with OtherInt types
    template <typename OtherInt>
    constexpr auto operator=(Strong<OtherInt, Tag> val) noexcept -> Strong & {
        m_val = val.get();
        return *this;
    }

    // Delete Assignment with OtherTag types
    template <typename OtherInt, typename OtherTag,
              std::enable_if_t<!std::is_same_v<Tag, OtherTag>>>
    auto operator=(Strong<OtherInt, OtherTag>) -> Strong & = delete;

    /* explicit */ constexpr operator Int() const noexcept { return m_val; } // NOLINT

    // Enable Coversion from OtherTag types
    template <typename OtherInt>
    /* explicit */ constexpr operator Strong<OtherInt, Tag>() noexcept { // NOLINT
        return m_val;
    }

    // Delete Coversion to OtherTag types
    template <typename OtherInt, typename OtherTag>
    constexpr operator Strong<OtherInt, OtherTag>() = delete;

    [[nodiscard]] constexpr auto get() const noexcept { return m_val; }

    [[nodiscard]] constexpr auto ptr() const noexcept { return &m_val; }
    [[nodiscard]] constexpr auto ptr() noexcept { return &m_val; }

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

    constexpr auto operator+() const noexcept { return *this; }
    constexpr auto operator-() const noexcept { return Strong<decltype(-m_val), Tag>(-m_val); }

    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(+);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(-);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(*);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(/);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(%);

    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(<<);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(>>);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(|);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(&);
    PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR(^);

  private:
    Int m_val = 0;
};

PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(+);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(-);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(*);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(/);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(%);

PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(|);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(&);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(^);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(<<);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR(>>);

PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(==);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(!=);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(<);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(>);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(>=);
PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR(<=);

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
        return boost::hash<base_type>()(i);
    }
};
} // namespace std

#undef ResType
#undef PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_OPERATOR
#undef PGSLAVE_STRONG_INTEGER_DEFINE_BINARY_COMPARISON_OPERATOR
#undef PGSLAVE_STRONG_INTEGER_DEFINE_COMPOUND_ASSIGNMENT_OPERATOR
