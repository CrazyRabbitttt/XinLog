#include "logConfig.h"

using namespace xinlog::detail;

// Return the instance of config 
Config &Config::instance() {
    static Config instance;
    return instance;
}

