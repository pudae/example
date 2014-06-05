namespace * test.protocol.room

struct ReqJoin
{
  1: required int     gsn
  2: required string  buffer
}

struct ReqLeave
{
  1: required int     gsn
  2: required string  buffer
}
