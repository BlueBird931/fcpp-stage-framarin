// Copyright © 2025 Giorgio Audrito. All Rights Reserved.

#include <array>

#include "lib/data/placed.hpp"

#include "test/helper.hpp"


using namespace fcpp;


TEST(PlacedTest, ExtractTier) {
    EXPECT_EQ((extract_tier<int, double>), 0);
    EXPECT_EQ((extract_tier<int, placed<8,char>, double>), 8);
    EXPECT_EQ((extract_tier<int, placed<8,char>, double, placed<16,float>>), 8);
    EXPECT_EQ((extract_tier<int, std::tuple<placed<8,char>, double>, float>), 8);
    EXPECT_EQ((extract_tier<int, std::array<placed<8,char>, 4>, float>), 8);
    EXPECT_EQ((extract_tier<int, placed<8,char> const&, double>), 8);
}

TEST(PlacedTest, IsPlaced) {
    constexpr tier_t tier = 8;
    
    EXPECT_EQ((tier_inf<7, 14, 28>), 4);
    EXPECT_EQ((tier_sup<7, 14, 28>), 31);
    
    bool b;
    b = is_placed<placed<tier, double>>;
    EXPECT_TRUE(b);
    b = is_placed<placed<tier, double> const>;
    EXPECT_TRUE(b);
    b = is_placed<placed<tier, double>&>;
    EXPECT_TRUE(b);
    b = is_placed<placed<tier, double>&&>;
    EXPECT_TRUE(b);
    b = is_placed<placed<tier, double, 12, 3> const&>;
    EXPECT_TRUE(b);
    b = is_placed<placed<tier, double> const&&>;
    EXPECT_TRUE(b);
    b = is_placed<int>;
    EXPECT_FALSE(b);
    b = is_placed<int const>;
    EXPECT_FALSE(b);
    b = is_placed<int&>;
    EXPECT_FALSE(b);
    b = is_placed<int const&>;
    EXPECT_FALSE(b);
    b = is_placed<int&&>;
    EXPECT_FALSE(b);
    b = is_placed<int const&&>;
    EXPECT_FALSE(b);
    b = is_placed<std::array<placed<tier, double>,4>>;
    EXPECT_TRUE(b);
    b = is_placed<std::array<placed<tier, double>,4> const>;
    EXPECT_TRUE(b);
    b = is_placed<std::array<int,4>>;
    EXPECT_FALSE(b);
    b = is_placed<std::array<int,4> const>;
    EXPECT_FALSE(b);
    b = is_placed<std::pair<placed<tier, double>,int>>;
    EXPECT_TRUE(b);
    b = is_placed<std::pair<placed<tier, double>,int> const>;
    EXPECT_TRUE(b);
    b = is_placed<std::pair<int,double>>;
    EXPECT_FALSE(b);
    b = is_placed<std::pair<int,double> const>;
    EXPECT_FALSE(b);
    b = is_placed<std::tuple<placed<tier, double>,char>>;
    EXPECT_TRUE(b);
    b = is_placed<std::tuple<placed<tier, double, 3, 12>,char> const>;
    EXPECT_TRUE(b);
    b = is_placed<std::tuple<int,char>>;
    EXPECT_FALSE(b);
    b = is_placed<std::tuple<int,char> const>;
    EXPECT_FALSE(b);
    b = is_placed<std::array<std::tuple<std::array<placed<tier, double>,3>,char>,4>>;
    EXPECT_TRUE(b);
    b = is_placed<std::array<std::tuple<std::array<placed<tier, double>,3>,char>,4> const&>;
    EXPECT_TRUE(b);
    b = is_placed<std::array<std::tuple<std::array<placed<tier, double>,3>,char>,4>&&>;
    EXPECT_TRUE(b);
    b = is_placed<std::array<std::tuple<std::array<double,3>,char>,4>>;
    EXPECT_FALSE(b);
    b = is_placed<std::array<std::tuple<std::array<double,3>,char>,4> const&>;
    EXPECT_FALSE(b);
}

TEST(PlacedTest, ToPlaced) {
    constexpr tier_t tier = 8;

    #define G(...) __VA_ARGS__
    #define CHECK_PLACED(V,P,Q,C) {                                         \
        using res = C;                                                      \
        using exp = V;                                                      \
        using T = placed<tier, std::decay_t<exp>, tier_t(P), tier_t(Q)>;    \
        EXPECT_SAME(typename res::value_type, exp);                         \
        EXPECT_SAME(typename res::type, T);                                 \
    }
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, double>));
    CHECK_PLACED(double&,       -1, 0, G(to_placed<tier, double&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, double&&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, double const>));
    CHECK_PLACED(double const&, -1, 0, G(to_placed<tier, double const&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, double const&&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double>>));
    CHECK_PLACED(double&,       12, 6, G(to_placed<tier, placed<tier, double, 12, 6>&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double>&&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double> const>));
    CHECK_PLACED(double const&, -1, 0, G(to_placed<tier, placed<tier, double> const&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double> const&&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double const>>));
    CHECK_PLACED(double const&, -1, 0, G(to_placed<tier, placed<tier, double const>&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double const>&&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double const> const>));
    CHECK_PLACED(double const&, -1, 0, G(to_placed<tier, placed<tier, double const> const&>));
    CHECK_PLACED(double,        -1, 0, G(to_placed<tier, placed<tier, double const> const&&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double>>));
    CHECK_PLACED(double&,       -1,-1, G(to_placed<tier, field<double>&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double>&&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double> const>));
    CHECK_PLACED(double const&, -1,-1, G(to_placed<tier, field<double> const&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double> const&&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double const>>));
    CHECK_PLACED(double const&, -1,-1, G(to_placed<tier, field<double const>&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double const>&&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double const> const>));
    CHECK_PLACED(double const&, -1,-1, G(to_placed<tier, field<double const> const&>));
    CHECK_PLACED(double,        -1,-1, G(to_placed<tier, field<double const> const&&>));
    CHECK_PLACED(G(std::pair<double,int> const&),               -1, 0,
                 G(to_placed<tier, placed<tier, std::pair<double,int>> const&>));
    CHECK_PLACED(G(std::pair<double,int>),                       4, 7,
                 G(to_placed<tier, std::pair<placed<tier, double const, 12, 6>, placed<tier, int, 6, 3>>>));
    CHECK_PLACED(G(std::pair<double,int>),                      12, 6,
                 G(to_placed<tier, std::pair<placed<tier, double const, 12, 6>, int>>));
    CHECK_PLACED(G(std::pair<double,int>),                      12, 6,
                 G(to_placed<tier, std::pair<placed<tier, double const, 12, 6>, int>&&>));
    CHECK_PLACED(G(std::pair<double const&,int const&>),        12, 6,
                 G(to_placed<tier, std::pair<placed<tier, double const, 12, 6>, int> const&>));
    CHECK_PLACED(G(std::pair<double,int const&>),               12, 6,
                 G(to_placed<tier, std::pair<placed<tier, double const, 12, 6>, int const&>&&>));
    CHECK_PLACED(G(std::pair<double,int>&),                     -1, 0,
                 G(to_placed<tier, std::pair<double,int>&>));
    CHECK_PLACED(G(std::array<double,4>),                        4, 7,
                 G(to_placed<tier, std::array<placed<tier, double, 4, 7>,4>>));
    CHECK_PLACED(G(std::array<double,4>),                        7, 4,
                 G(to_placed<tier, placed<tier, std::array<double,4>, 7, 4>>));
    CHECK_PLACED(G(std::array<double,4>),                       -1, 0,
                 G(to_placed<tier, std::array<placed<tier, double>&,4>>));
    CHECK_PLACED(G(std::array<double,4>),                       -1, 0,
                 G(to_placed<tier, std::array<placed<tier, double>,4>&>));
    CHECK_PLACED(G(std::tuple<double,int>),                     -1, 0,
                 G(to_placed<tier, std::tuple<double,int>>));
    CHECK_PLACED(G(std::tuple<int&,char>),                      -1, -1,
                 G(to_placed<tier, std::tuple<field<int>&,char>>));
    CHECK_PLACED(G(std::tuple<double,char>),                    12, -1,
                 G(to_placed<tier, std::tuple<placed<tier, double, 12, 6>, field<char>>>));
    CHECK_PLACED(G(std::array<std::tuple<std::array<double,3>, char>,4>), 6, -1,
                 G(to_placed<tier, std::array<std::tuple<std::array<placed<tier, double, 6, 7>,3>&, field<char>>,4>>));
}

TEST(PlacedTest, Constructors) {
    placed<8,double,255,0> fd, x, y;
    placed<8,int,11,6> x2(x), x3(std::move(y));
    x2 = fd;
    x.get();
    x.get_or(3);
    x2.get_or(3);
}

TEST(PlacedTest, PMapHood) {
    placed<8,int> x(1);
    placed<8,int,11,6> y(2);
    placed<8,int,12,12> z(4);
    placed<8,int,6> w;
    auto r1 = pmap_hood([](int a, int b){
        return a+b;
    }, x, y);
    EXPECT_SAME(decltype(r1), placed<8,int,11,6>);
    EXPECT_EQ(r1.get_or(999), 3);
    auto r2 = pmap_hood([](int a, int b, int c){
        return a+b+c;
    }, x, y, z);
    EXPECT_SAME(decltype(r2), placed<8,int,8,14>);
    EXPECT_EQ(r2.get_or(999), 7);
    auto r3 = pmap_hood([](int a, int b, int c){
        return a+b+c;
    }, field<int>(1), 8, z);
    EXPECT_SAME(decltype(r3), placed<8,int,12,tier_t(-1)>);
    EXPECT_EQ(r3.get_or(999), 13);
    auto r4 = pmap_hood([](int a, int b, int c){
        return a+b+c;
    }, x, field<int>(8), z);
    EXPECT_SAME(decltype(r4), placed<8,int,12,tier_t(-1)>);
    EXPECT_EQ(r4.get_or(999), 13);
    auto r5 = pmap_hood([](int a, int b){
        return a+b;
    }, 4, field<int>(8));
    EXPECT_SAME(decltype(r5), field<int>);
    EXPECT_EQ(r5, field<int>(12));
    auto r6 = pmap_hood([](int a, int b, int c){
        return a+b+c;
    }, x, field<int>(8), w);
    EXPECT_SAME(decltype(r6), placed<8,int,6,tier_t(-1)>);
    EXPECT_EQ(r6.get_or(999), 999);
}

TEST(PlacedTest, Operators) {
    placed<8, int, 12, 2> x(1);
    placed<8, double, 24, 4> y(2);

    auto r1 = x + x;
    EXPECT_SAME(decltype(r1), placed<8, int, 12, 2>);
    EXPECT_EQ(r1.get_or(999), 2);
    auto r2 = x + y;
    EXPECT_SAME(decltype(r2), placed<8, double, 8, 6>);
    EXPECT_EQ(r2.get_or(999), 3.0);
    auto r3 = x + 3;
    EXPECT_SAME(decltype(r3), placed<8, int, 12, 2>);
    EXPECT_EQ(r3.get_or(999), 4);
    auto r4 = 3 + y;
    EXPECT_SAME(decltype(r4), placed<8, double, 24, 4>);
    EXPECT_EQ(r4.get_or(999), 5.0);
    auto r5 = field<int>(5) + x;
    EXPECT_SAME(decltype(r5), placed<8, int, 12, tier_t(-1)>);
    EXPECT_EQ(r5.get_or(999), 6);
    auto r6 = (x * y) / 2;
    EXPECT_SAME(decltype(r6), placed<8, double, 8, 6>);
    EXPECT_EQ(r6.get_or(999), 1.0);
    auto r7 = -x;
    EXPECT_SAME(decltype(r7), placed<8, int, 12, 2>);
    EXPECT_EQ(r7.get_or(999), -1);
}

TEST(PlacedTest, FoldHood) {
    auto x = details::make_placed<8, double, 12, 2>({1, 2, 3}, {2, 4, 6, 8});
    auto r1 = details::fold_hood([] (double i, double j) {return i+j;}, x, {0,1,2});
    EXPECT_SAME(decltype(r1), placed<8, double, 12, 0>);
    EXPECT_EQ(r1.get_or(999), 12.0);
    auto r2 = details::fold_hood([] (double i, double j) {return i+j;}, x, 5, {0,1,2}, 2);
    EXPECT_SAME(decltype(r2), placed<8, double, 12, 0>);
    EXPECT_EQ(r2.get_or(999), 11.0);
}
