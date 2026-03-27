// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <array>
#include <algorithm>

namespace Kurveball
{
    template<typename ContainedT, size_t Size>
    class CircularBuffer
    {
        static_assert(Size > 0U);
    public:
        using ValueType = ContainedT;
        using ConstIteratorT = typename std::array<ContainedT, Size>::const_iterator;
        CircularBuffer();

        void Clear();
        
        bool AddToEnd(const ContainedT& item);
        
        // Gets the number of buffer entries that are actually saved in the buffer so far. May
        // be smaller than the overall Size.
        size_t GetNumContained() const;

        // relativeIndex is 1-indexed and relative to the buffer's cursor position on the ring.
        // 1 is the most recently written item, 2 is the second-newest, etc. No need to worry about overflow,
        // as the cursor wraps around the boundary of the ring.
        const ContainedT& PeekNewest(size_t relativeIndex) const;

        // Like PeekNewest, but 1 is the oldest item, 2 is the second-oldest, etc. In other words,
        // nth oldest = mWriteCursor + n - 1
        const ContainedT& PeekOldest(size_t relativeIndex) const;

        ConstIteratorT Begin() const;

        ConstIteratorT End() const;

    private:
        size_t WrapCursor(size_t absoluteCursor) const;
        bool IsCursorValid(size_t absoluteCursor) const;

        std::array<ContainedT, Size> mRing;
        size_t mWriteCursor{ 0U };
        bool mHasWrapped{ false };
        bool mHasUpdated{ false };
    };
}

#include "CircularBuffer.inl"