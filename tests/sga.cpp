/* Copyright 2017 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#define BOOST_TEST_MODULE sga_problem_test
#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/sea.hpp>
#include <pagmo/algorithms/sga.hpp>
#include <pagmo/io.hpp>
#include <pagmo/population.hpp>
#include <pagmo/problems/hock_schittkowsky_71.hpp>
#include <pagmo/problems/inventory.hpp>
#include <pagmo/problems/rosenbrock.hpp>
#include <pagmo/problems/schwefel.hpp>
#include <pagmo/problems/zdt.hpp>
#include <pagmo/serialization.hpp>
#include <pagmo/types.hpp>

using namespace pagmo;

BOOST_AUTO_TEST_CASE(sga_algorithm_construction)
{
    // sga uda{1u, 0.5, 10., .02, .5, 1u, 2u, "gaussian", "truncated", "sbx", 0u};
    // We check the default constructor, a correct call and the possibility to build a pagmo::algorithm
    BOOST_CHECK_NO_THROW(sga{});
    BOOST_CHECK_NO_THROW((sga{1u, .95, 10., .02, .5, 5u, "exponential", "gaussian", "tournament", 0u, 32u}));
    BOOST_CHECK_NO_THROW((sga{1u, .95, 10., .02, .5, 5u, "exponential", "uniform", "tournament", 0u, 32u}));
    BOOST_CHECK_NO_THROW((sga{1u, .95, 10., .02, 20., 5u, "exponential", "polynomial", "tournament", 0u, 32u}));
    BOOST_CHECK_NO_THROW((sga{1u, .95, 10., .02, .5, 5u, "exponential", "gaussian", "truncated", 0u, 32u}));
    BOOST_CHECK_NO_THROW((sga{1u, .95, 10., .02, .5, 5u, "binomial", "gaussian", "tournament", 0u, 32u}));
    BOOST_CHECK_NO_THROW((sga{1u, .95, 10., .02, .5, 5u, "sbx", "gaussian", "tournament", 0u, 32u}));
    BOOST_CHECK_NO_THROW(algorithm(sga{}));
    // We check incorrect calls to the constructor
    BOOST_CHECK_THROW((sga{1u, 12., 10., .02, .5, 5u, "exponential", "gaussian", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, -1.1, 10., .02, .5, 5u, "exponential", "gaussian", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 0.1, .02, .5, 5u, "exponential", "gaussian", "truncated", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 101., .02, .5, 5u, "exponential", "gaussian", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., -0.2, .5, 5u, "exponential", "gaussian", "truncated", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., 1.3, .5, 5u, "exponential", "gaussian", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 5u, "exponential", "unknown_method", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 5u, "exponential", "gaussian", "unknown_method", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 5u, "unknown_method", "gaussian", "truncated", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 5u, "exponential", "polynomial", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, 101, 5u, "exponential", "polynomial", "truncated", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, -3, 5u, "exponential", "uniform", "tournament", 0u, 32u}),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, 1.1, 5u, "exponential", "uniform", "tournament", 0u, 32u}),
                      std::invalid_argument);
}
BOOST_AUTO_TEST_CASE(sga_evolve_test)
{
    // We start testing the various throws in case the evolve is called on a population
    // not suitable for sga
    BOOST_CHECK_THROW((sga{}.evolve(population{zdt{}, 5u, 23u})), std::invalid_argument);
    BOOST_CHECK_THROW((sga{}.evolve(population{hock_schittkowsky_71{}, 5u, 23u})), std::invalid_argument);
    BOOST_CHECK_THROW((sga{}.evolve(population{schwefel{20u}, 1u, 23u})), std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 21u, "exponential", "gaussian", "tournament", 0u, 32u}.evolve(
                          population{schwefel{20u}, 20u, 23u})),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 2u, "sbx", "gaussian", "tournament", 0u, 32u}.evolve(
                          population{schwefel{20u}, 25u, 23u})),
                      std::invalid_argument);
    BOOST_CHECK_THROW((sga{1u, .95, 10., .02, .5, 2u, "sbx", "gaussian", "tournament", 35u, 32u}.evolve(
                          population{schwefel{20u}, 24u, 23u})),
                      std::invalid_argument);
    // The we test that evolution is deterministic if the
    // seed is controlled.
    std::vector<sga> udps = {
        sga{10u, .90, 1., 0.1, 1., 2u, "exponential", "gaussian", "tournament", 10u, 32u},
        sga{10u, .90, 1., 0.1, 1., 2u, "sbx", "gaussian", "tournament", 10u, 32u},
        sga{10u, .90, 1., 0.1, 1., 2u, "binomial", "gaussian", "tournament", 10u, 32u},
        sga{10u, .90, 1., 0.1, 1., 2u, "single", "gaussian", "tournament", 10u, 32u},
        sga{10u, .90, 1., 0.1, 1., 2u, "exponential", "polynomial", "tournament", 10u, 32u},
        sga{10u, .90, 1., 0.1, 1., 2u, "exponential", "uniform", "tournament", 10u, 32u},
        sga{10u, .90, 1., 0.1, 1., 2u, "exponential", "gaussian", "truncated", 10u, 32u},
    };
    // On a deterministic problem
    for (sga &udp : udps) {
        problem prob{schwefel{20u}};
        population pop1{prob, 20u, 23u};
        udp.set_seed(32u);
        udp.set_verbosity(1u);
        pop1 = udp.evolve(pop1);
        auto log1 = udp.get_log();

        population pop2{prob, 20u, 23u};
        udp.set_seed(32u);
        pop2 = udp.evolve(pop2);
        auto log2 = udp.get_log();

        BOOST_CHECK(log1 == log2);
    }
    // And on a stochastic one
    for (sga &udp : udps) {
        problem prob{inventory{25u, 5u, 1432u}};
        population pop1{prob, 20u, 23u};
        udp.set_seed(32u);
        udp.set_verbosity(1u);
        pop1 = udp.evolve(pop1);
        auto log1 = udp.get_log();

        population pop2{prob, 20u, 23u};
        udp.set_seed(32u);
        pop2 = udp.evolve(pop2);
        auto log2 = udp.get_log();

        BOOST_CHECK(log1 == log2);
    }
}

BOOST_AUTO_TEST_CASE(sga_serialization_test)
{
    print("HERE\n");
    // Make one evolution
    problem prob{schwefel{20u}};
    population pop{prob, 20u, 23u};
    algorithm algo{sga{10u}};
    algo.set_verbosity(1u);
    pop = algo.evolve(pop);

    // Store the string representation of p.
    std::stringstream ss;
    auto before_text = boost::lexical_cast<std::string>(algo);
    auto before_log = algo.extract<sga>()->get_log();
    // Now serialize, deserialize and compare the result.
    {
        cereal::JSONOutputArchive oarchive(ss);
        oarchive(algo);
    }
    // Change the content of p before deserializing.
    algo = algorithm{null_algorithm{}};
    {
        cereal::JSONInputArchive iarchive(ss);
        iarchive(algo);
    }
    auto after_text = boost::lexical_cast<std::string>(algo);
    auto after_log = algo.extract<sga>()->get_log();
    BOOST_CHECK_EQUAL(before_text, after_text);
    // BOOST_CHECK(before_log == after_log); // This fails because of floating point problems when using JSON and cereal
    // so we implement a close check

    for (auto i = 0u; i < before_log.size(); ++i) {
        BOOST_CHECK_EQUAL(std::get<0>(before_log[i]), std::get<0>(after_log[i]));
        BOOST_CHECK_EQUAL(std::get<1>(before_log[i]), std::get<1>(after_log[i]));
        BOOST_CHECK_CLOSE(std::get<2>(before_log[i]), std::get<2>(after_log[i]), 1e-8);
        BOOST_CHECK_CLOSE(std::get<3>(before_log[i]), std::get<3>(after_log[i]), 1e-8);
    }
}
