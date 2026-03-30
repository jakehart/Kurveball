// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <cassert>
#include <functional>

namespace Kurveball
{
    enum class ErrorCode : uint8_t
    {
        None = 0,
        UnknownError,
        CurveNotFound,
        InvalidCurveAsset,
        TimeReversal,
        SpeedSamplerNotFound,
        BlendTypeNotFound
    };

    // User-defined error handler function
    using ErrorHandler = std::function<void(ErrorCode)>;
}

#define KURVEBALL_VERIFY_RETURN(criterion, ...) { assert((criterion)); if(!(criterion)) return __VA_ARGS__; }

#define KURVEBALL_ERROR_RETURN(criterion, curveContext, errorCode, ...) { \
    if(!(criterion)) \
    { \
        curveContext.mLastErrorCode = errorCode; \
        if(curveContext.mErrorHandler) \
        { \
            curveContext.mErrorHandler(errorCode); \
        } \
        assert((criterion)); \
        return __VA_ARGS__; \
    } \
}