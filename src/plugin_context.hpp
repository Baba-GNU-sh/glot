#pragma once

#include "database.hpp"

class PluginContext
{
  public:
    PluginContext(Database &database);
    Database &get_database();

  private:
    Database &_database;
};
