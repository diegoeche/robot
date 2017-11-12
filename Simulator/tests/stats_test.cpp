#define BOOST_TEST_MODULE Stats Test

#include <boost/test/included/unit_test.hpp>

#include "stats.h"

BOOST_AUTO_TEST_CASE(Stats_Constructors_Test) {
  auto stats = Stats(1,2,3);

  BOOST_CHECK_EQUAL(stats.getAttack(),  1);
  BOOST_CHECK_EQUAL(stats.getDefence(), 2);
  BOOST_CHECK_EQUAL(stats.getArmor(),   3);

  auto stats2 = Stats(1,2);

  BOOST_CHECK_EQUAL(stats2.getAttack(),  1);
  BOOST_CHECK_EQUAL(stats2.getDefence(), 2);
  // Defaults to 0 Armor
  BOOST_CHECK_EQUAL(stats2.getArmor(),   0);
}

BOOST_AUTO_TEST_CASE(Stats_CalculateDamage_test) {
  auto attacker = Stats(1,2);
  auto defender = Stats(2,1);

  BOOST_CHECK_EQUAL(attacker.calculateDamage(defender), 1);
  BOOST_CHECK_EQUAL(defender.calculateDamage(attacker), 1);
}
