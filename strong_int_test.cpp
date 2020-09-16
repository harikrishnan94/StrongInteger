#include <boost/static_string.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>

#include "strong_integer.h"

using namespace pgslave::util;

using Int1 = Strong<uint64_t, struct Int1Tag>;
using Int2 = Strong<int, struct Int2Tag>;

void func(Int2);
void func(int);
void func(Int1);

template <> struct fmt::formatter<Int1> : fmt::formatter<std::string_view> {
    template <typename FormatContext> auto format(Int1 rec, FormatContext &ctx) {
        constexpr auto MAX_XLOG_REC_LEN = 20;
        boost::static_string<MAX_XLOG_REC_LEN> out;

        fmt::format_to_n(std::back_inserter(out), MAX_XLOG_REC_LEN, FMT_STRING("{:X}/{:X}"),
                         uint32_t(rec.get() >> 32U), uint32_t(rec.get()));
        return formatter<std::string_view>::format({out.data(), out.size()}, ctx);
    }
};

auto operator<<(std::ostream &os, Int1 i) -> std::ostream & { return os << i.get(); }

auto main() -> int {
    Int1 a = 8074984;
    Int1 b = 37409574;
    auto c = a + b + 1;
    c += 3;

    static_assert(std::is_same_v<decltype(a), decltype(c)>);

    fmt::print(FMT_STRING("{}\n"), a << 1);
    fmt::printf("%lu\n", a << 1);
}