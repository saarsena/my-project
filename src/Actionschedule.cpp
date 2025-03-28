// ActionSchedule.cpp
#include "ActionSchedule.hpp"

bool ActionSchedule::operator<(const ActionSchedule &a) const {
  return tick > a.tick;
}