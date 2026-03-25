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
            if (!mHasUpdated)
            {
                return mRing.cbegin();
            }

            size_t count = GetNumContained();
            if (count == 0)
            {
                return mRing.cbegin();
            }

            // The oldest element is at (mWriteCursor - count) wrapped around
            // Example: Size=5, Write=2, Count=3. Oldest is at (2-3)%5 = -1 -> 4.
            // Wait, let's re-verify the "oldest" definition.
            // In AddToEnd: mWriteCursor points to the NEXT slot to write.
            // So the LAST written item is at (mWriteCursor - 1).
            // The FIRST written item (oldest) is at (mWriteCursor - count).

            // Since we are dealing with unsigned size_t, we need to handle the wrap carefully.
            // (a - b) % N is tricky with unsigned if a < b.
            // Better: (mWriteCursor + Size - count) % Size

            size_t oldestIndex = (mWriteCursor + Size - count) % Size;

            ConstIteratorT iter = mRing.cbegin();
            std::advance(iter, oldestIndex);
            return iter;
        }

        ConstIteratorT End() const
        {
            if (!mHasUpdated)
            {
                return mRing.cbegin();
            }

            size_t count = GetNumContained();
            if (count == 0)
            {
                return mRing.cbegin();
            }

            // The end iterator is the slot AFTER the newest element.
            // The newest element is at (mWriteCursor - 1).
            // So the end is at mWriteCursor.
            // We just need to wrap it if necessary, though mWriteCursor is already wrapped.

            ConstIteratorT iter = mRing.cbegin();
            std::advance(iter, mWriteCursor);
            return iter;
        }

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