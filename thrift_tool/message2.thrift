namespace * test.protocol.room

struct AnsJoin
{
  1: required int     gsn
  2: required string  buffer
}

struct AnsLeave
{
  1: required int     gsn
  2: required string  buffer
}
