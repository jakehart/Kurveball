// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <assert.h>

#define CURVELIB_VERIFY_RETURN(criterion, ...) { assert((criterion)); if(!(criterion)) return __VA_ARGS__; }