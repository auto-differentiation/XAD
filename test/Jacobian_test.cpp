/*******************************************************************************

   Tests for xad::Jacobian class.

   This file is part of XAD, a comprehensive C++ library for
   automatic differentiation.

   Copyright (C) 2010-2024 Xcelerit Computing Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#include <XAD/XAD.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <functional>
#include <vector>

template <typename T>
T foo1(std::vector<T> &x)
{
    return x[0] + sin(x[1]);
}

template <typename T>
T foo2(std::vector<T> &x)
{
    return x[1] + sin(x[0]);
}

template <typename T>
std::vector<T> foo(std::vector<T> &x)
{
    return {foo1(x), foo2(x)};
}

TEST(JacobianTest, SimpleAdjoint)
{
    typedef xad::adj<double> mode;
    typedef mode::tape_type tape_type;
    typedef mode::active_type AD;

    tape_type tape;

    std::vector<AD> x = {3, 1};
    xad::Jacobian<AD> jac(foo<AD>, x, &tape);

    std::vector<std::vector<AD>> cross_jacobian = {{1.0, cos(x[0])}, {cos(x[1]), 1.0}};
    std::vector<std::vector<AD>> computed_jacobian = jac.get();

    for (unsigned int i = 0; i < cross_jacobian.size(); i++)
        for (unsigned int j = 0; j < cross_jacobian.size(); j++)
            ASSERT_EQ(cross_jacobian[i][j], computed_jacobian[i][j]);
}

TEST(JacobianTest, SimpleAdjointIterator)
{
    typedef xad::adj<double> mode;
    typedef mode::tape_type tape_type;
    typedef mode::active_type AD;

    tape_type tape;

    std::vector<AD> x = {3, 1};
    std::list<std::list<AD>> computed_jacobian(2.0, std::list<AD>(2.0, 0.0));
    xad::Jacobian<AD> jac(foo<AD>, x, &tape, computed_jacobian.begin(), computed_jacobian.end());

    std::list<std::list<AD>> cross_jacobian = {{1.0, cos(x[0])}, {cos(x[1]), 1.0}};

    auto row1 = computed_jacobian.begin(), row2 = cross_jacobian.begin();
    while (row1 != computed_jacobian.end() && row2 != cross_jacobian.end())
    {
        auto col1 = row1->begin(), col2 = row2->begin();
        while (col1 != row1->end() && col2 != row2->end())
        {
            ASSERT_EQ(*col1, *col2);
            col1++;
            col2++;
        }
        row1++;
        row2++;
    }
}

TEST(JacobianTest, SimpleForward)
{
    typedef xad::fwd<double> mode;
    typedef mode::active_type AD;

    std::vector<AD> x = {-2, 1};
    xad::Jacobian<AD> jac(foo<AD>, x);

    std::vector<std::vector<AD>> cross_jacobian = {{1.0, cos(-2)}, {cos(1), 1.0}};
    std::vector<std::vector<AD>> computed_jacobian = jac.get();

    for (unsigned int i = 0; i < cross_jacobian.size(); i++)
        for (unsigned int j = 0; j < cross_jacobian.size(); j++)
            ASSERT_EQ(cross_jacobian[i][j], computed_jacobian[i][j]);
}

TEST(JacobianTest, SimpleForwardIterator)
{
    typedef xad::fwd<double> mode;
    typedef mode::active_type AD;

    std::vector<AD> x = {-2, 1};
    std::list<std::list<AD>> computed_jacobian(2.0, std::list<AD>(2.0, 0.0));
    xad::Jacobian<AD> jac(foo<AD>, x, computed_jacobian.begin(), computed_jacobian.end());

    std::list<std::list<AD>> cross_jacobian = {{1.0, cos(-2)}, {cos(1), 1.0}};

    auto row1 = computed_jacobian.begin(), row2 = cross_jacobian.begin();
    while (row1 != computed_jacobian.end() && row2 != cross_jacobian.end())
    {
        auto col1 = row1->begin(), col2 = row2->begin();
        while (col1 != row1->end() && col2 != row2->end())
        {
            ASSERT_EQ(*col1, *col2);
            col1++;
            col2++;
        }
        row1++;
        row2++;
    }
}
