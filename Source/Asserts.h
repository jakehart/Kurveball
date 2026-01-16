// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <assert.h>

#define CURVELIB_VERIFY_RETURN(criterion, retVal) { assert((criterion)); if(!(criterion)) return retVal; }