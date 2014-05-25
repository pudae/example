#include "boost/test/unit_test.hpp"

#include "type_table.hpp"
#include "messages.hpp"

using namespace windup;

BOOST_AUTO_TEST_CASE(test_id_to_type)
{
  auto msg_a = TypeTable<MsgHolder>::type((int)TypeId::kMsgA);
  BOOST_REQUIRE(msg_a.which() == 0);
  auto msg_b = TypeTable<MsgHolder>::type((int)TypeId::kMsgB);
  BOOST_REQUIRE(msg_b.which() == 1);
  auto msg_c = TypeTable<MsgHolder>::type((int)TypeId::kMsgC);
  BOOST_REQUIRE(msg_c.which() == 2);
}
