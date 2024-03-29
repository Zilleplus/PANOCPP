#define CATCH_CONFIG_MAIN
#include <panoc/VectorAlgebra.hpp>
#include <catch.hpp>

using namespace pnc;

TEST_CASE("matrix tests")
{
    pnc::Matrix<int, int> a(2, 2);
    a[0][0] = 1;
    a[0][1] = 2;
    a[1][0] = 3;
    a[1][1] = 4;

    pnc::Matrix<int, int> b(2, 2);
    b[0][0] = 1;
    b[0][1] = 1;
    b[1][0] = 2;
    b[1][1] = 2;

    SECTION("create simple matrix, access the elements")
    {
        REQUIRE(a[0][0]==1);
        REQUIRE(a[0][1]==2);
        REQUIRE(a[1][0]==3);
        REQUIRE(a[1][1]==4);
    }

    SECTION("mul collums together")
    {
        const int first = a[0]*b[0];
        REQUIRE(first == (1*1) + (2*1));
    }

    SECTION("add collums together")
    {
        pnc::Vector<int> vec(2);
        vec = a[0] + b[0];
        REQUIRE(vec[0] == 2);
        REQUIRE(vec[1] == 3);
    }

    SECTION("save col in vector")
    {
        pnc::Vector<int> vec(2);
        vec = a[0];
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == 2);
    }

    SECTION("copy vector into matrix")
    {
        pnc::Vector<int, int> vec = {1,2};
        pnc::Matrix<int, int> m(2, 2);
        m[0][0] = -1;
        m[0][1] = -1;

        m[0] = vec;
        REQUIRE(m[0][0] == 1);
        REQUIRE(m[0][1] == 2);
    }
}

TEST_CASE("vector algebra test")
{
    pnc::Vector<double> vector1 =  { 1, 2 };
    pnc::Vector<double> vector2 =  { 3, 4 };
    SECTION("vector sum")
    {
        auto vectorSum = vector1 + vector2;

        REQUIRE(vectorSum[0] == 4);
        REQUIRE(vectorSum[1] == 6);
    }

    SECTION("vector product")
    {
        auto vectorProd = vector1 * vector2;

        REQUIRE(vectorProd == 11);
    }

    SECTION("vector negative")
    {
        auto res = -(vector1);

        REQUIRE(res[0]==-1);
        REQUIRE(res[1]==-2);
    }

    SECTION("Add right vector to left constant")
    {
        double c = 2.0;
        auto res = c + vector1;

        REQUIRE(res[0]==3);
        REQUIRE(res[1]==4);
    }

    SECTION("Add left vector to right constant")
    {
        double c = 2.0;
        auto res = vector1 + c;

        REQUIRE(res[0]==3);
        REQUIRE(res[1]==4);
    }

    SECTION("Mul right vector to left constant")
    {
        double c = 2.0;
        auto res = c * vector1;

        REQUIRE(res[0]==2);
        REQUIRE(res[1]==4);
    }

    SECTION("Mul left vector to right constant")
    {
        double c = 2.0;
        auto res = vector1 * c;

        REQUIRE(res[0]==2);
        REQUIRE(res[1]==4);
    }

    SECTION("Multi operation")
    {
        auto x = pnc::Vector<int>{1,2};
        auto y = pnc::Vector<int>{3,4};
        auto c = 2;
        auto res = (x * c) + y;

        REQUIRE(res[0]==5);
        REQUIRE(res[1]==8);
    }

    SECTION("Bug when combining stuff")
    {
        auto x = pnc::Vector<int>{1,2};
        auto y = pnc::Vector<int>{3,4};
        auto res = SUM(y+(-x)); // (3-1) + (4-2) = 2 + 2 = 4

        REQUIRE(res == 4);
    }
}

TEST_CASE("Helper methods")
{
    auto vec = pnc::Vector<double>{ 1.0, 2.0 };
    auto vecWithsign = pnc::Vector<double>{ -1.0, 2.0 };

    SECTION("SUM")
    {
        REQUIRE(SUM(vec) == 3);
    }

    SECTION("MAX")
    {
        REQUIRE(MAX(vec) == 2);
    }

    SECTION("L1 norm")
    {
        // l1-norm sum(|1|,|2|) = 3
        auto res = NormL1(vecWithsign);

        REQUIRE(res == 3);
    }
}
