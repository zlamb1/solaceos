#ifndef DATETIME_H
#define DATETIME_H 1

#include "common.h"

#define DAY_SECONDS 86400

struct datetime
{
  u32 year;
  u8 month;
  u8 day_of_month;
  u8 hour;
  u8 minute;
  u8 second;
};

static inline struct datetime
get_datetime (u64 unix_time)
{
  struct datetime dt = { 0 };
  u8 is_leap_year;

  dt.year = 1970;
  while (unix_time)
    {
      is_leap_year
          = dt.year % 4 == 0 && (dt.year % 100 != 0 || dt.year % 400 == 0);
      u64 required_time = is_leap_year ? 366 * DAY_SECONDS : 365 * DAY_SECONDS;

      if (unix_time < required_time)
        break;

      ++dt.year;
      unix_time -= required_time;
    }

#define ADVANCE_MONTH(days)                                                   \
  if (unix_time >= (days) * DAY_SECONDS)                                      \
    {                                                                         \
      ++dt.month;                                                             \
      unix_time -= (days) * DAY_SECONDS;                                      \
    }                                                                         \
  else                                                                        \
    break;

  do
    {
      ADVANCE_MONTH (31);
      ADVANCE_MONTH (is_leap_year ? 29 : 28);
      ADVANCE_MONTH (31);
      ADVANCE_MONTH (30);
      ADVANCE_MONTH (31);
      ADVANCE_MONTH (30);
      ADVANCE_MONTH (31);
      ADVANCE_MONTH (31);
      ADVANCE_MONTH (30);
      ADVANCE_MONTH (31);
      ADVANCE_MONTH (30);
      ADVANCE_MONTH (31);
    }
  while (0);

#undef ADVANCE_MONTH

  dt.day_of_month = unix_time / DAY_SECONDS;
  unix_time %= DAY_SECONDS;

  dt.hour   = unix_time / 3600;
  dt.minute = unix_time / 60 % 60;
  dt.second = unix_time % 60;

  return dt;
}

#endif