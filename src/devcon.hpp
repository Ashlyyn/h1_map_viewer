#pragma once

#include "com_defs.hpp"

           int         DevCon_Thread(void*);
           void        DevCon_Init();
NO_DISCARD bool        DevCon_HasText();
NO_DISCARD std::string DevCon_TakeText();
           void        DevCon_Shutdown();