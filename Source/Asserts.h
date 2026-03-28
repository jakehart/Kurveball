// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <assert.h>
#include <functional>

namespace Kurveball
{
    enum class ErrorCode : uint8_t
    {
        UnknownError = 0,
        CurveNotFound,
        InvalidCurveAsset,
        TimeReversal,
        SpeedSamplerNotFound
    };

    // User-defined error handler function
    using ErrorHandler = std::function<void(ErrorCode)>;
}

#define KURVEBALL_VERIFY_RETURN(criterion, ...) { assert((criterion)); if(!(criterion)) return __VA_ARGS__; }

#define KURVEBALL_ERROR_RETURN(criterion, curveContext, errorCode, ...) { \
    if(!(criterion)) \
    { \
        if(curveContext.mErrorHandler) \
        { \
            curveContext.mErrorHandler(errorCode); \
        } \
        assert((criterion)); \
        return __VA_ARGS__; \
    } \
}