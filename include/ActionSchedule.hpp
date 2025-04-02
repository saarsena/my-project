// ActionSchedule.h
#ifndef ACTIONSCHEDULE_H
#define ACTIONSCHEDULE_H

#include "Entity.hpp"

struct ActionSchedule {
  int tick;
  Entity *entity;
  bool operator<(const ActionSchedule &a) const;
};

#endif
