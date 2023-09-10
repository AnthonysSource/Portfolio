#pragma once

#include "optick.h"


#define CKE_PROFILE_START_FRAME(name) OPTICK_FRAME(name)
#define CKE_PROFILE_TAG(name, value) OPTICK_TAG(name, value)
#define CKE_PROFILE_EVENT() OPTICK_EVENT();