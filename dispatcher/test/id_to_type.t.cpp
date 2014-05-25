#include "boost/test/unit_test.hpp"

#include "id_to_type.hpp"
#include "messages.hpp"

BOOST_AUTO_TEST_CASE(test_id_to_type)
{
  auto msg_a = IdToType<MsgHolder>::type((int)TypeId::kMsgA);
  BOOST_REQUIRE(msg_a.which() == 0);
  auto msg_b = IdToType<MsgHolder>::type((int)TypeId::kMsgB);
  BOOST_REQUIRE(msg_b.which() == 1);
  auto msg_c = IdToType<MsgHolder>::type((int)TypeId::kMsgC);
  BOOST_REQUIRE(msg_c.which() == 2);
}
