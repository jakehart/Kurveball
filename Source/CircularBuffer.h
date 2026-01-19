// MIT NON-AI License. Copyright (c) 2025 Jake Hart. See LICENSE.md
#pragma once
#include <array>
#include <algorithm>

namespace CurveLib
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

        ConstIteratorT Begin() const
        {
            // Start with the array's absolute begin iterator
            ConstIteratorT startIter;

            // For const iterators, use cbegin()
            startIter = mRing.cbegin();

            // Brief comment: Determine the index of the oldest element
            size_t oldestIndex = 0;

            if (!mHasEverUpdated)
            {
                // Brief comment: If never updated, the buffer is empty, begin is end
                return startIter;
            }

            if (mHasWrapped)
            {
                // Brief comment: If wrapped, the oldest element is right after the current write cursor
                oldestIndex = mWriteCursor;
            }
            else
            {
                // Brief comment: If not wrapped, the oldest element is always at index 0
                oldestIndex = 0;
            }

            // Brief comment: Advance the iterator to the calculated index
            std::advance(startIter, oldestIndex);
            return startIter;
        }

        ConstIteratorT End() const
        {
            ConstIteratorT endIter;

            // Start with the array's absolute begin iterator
            endIter = mRing.cbegin();

            if (!mHasEverUpdated)
            {
                // If never updated, the buffer is empty, end is begin
                return endIter;
            }

            if (mHasWrapped)
            {
                // If wrapped, the newest element is just before the write cursor
                // The cursor points to the NEXT available slot, which is also the 'end' marker
                // The size of the active data is 'Size'
                std::advance(endIter, Size);
                return endIter;
            }
            else
            {
                // If not wrapped, the end marker is the current write cursor
                // The cursor points to the next available slot, which is the end marker
                std::advance(endIter, mWriteCursor);
                return endIter;
            }
        }

    private:
        size_t WrapCursor(size_t absoluteCursor) const;
        bool IsCursorValid(size_t absoluteCursor) const;

        std::array<ContainedT, Size> mRing;
        size_t mWriteCursor{ 0U };
        bool mHasWrapped{ false };
        bool mHasEverUpdated{ false };
    };
}

#include "CircularBuffer.inl"