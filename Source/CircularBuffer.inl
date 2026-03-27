namespace Kurveball
{
    template<typename ContainedT, size_t Size>
    CircularBuffer<ContainedT, Size>::CircularBuffer()
    {
        Clear();
    }

    template<typename ContainedT, size_t Size>
    void CircularBuffer<ContainedT, Size>::Clear()
    {
        mRing.fill({});
        mWriteCursor = 0ULL;
        mHasWrapped = false;
    }

    template<typename ContainedT, size_t Size>
    bool CircularBuffer<ContainedT, Size>::AddToEnd(const ContainedT& item)
    {
        if (!IsCursorValid(mWriteCursor))
        {
            return false;
        }

        // Cursor always points at the next item to be overwritten
        mRing[mWriteCursor] = item;

        if (mWriteCursor == Size - 1)
        {
            mHasWrapped = true;
        }

        // mWriteCursor always points to the next element we will write
        mWriteCursor = WrapCursor(mWriteCursor + 1);
        mHasUpdated = true;

        return true; // Success
    }
    
    template<typename ContainedT, size_t Size>
    size_t CircularBuffer<ContainedT, Size>::GetNumContained() const
    {
        if(!mHasUpdated)
        {
            return 0;
        }
        else if (!mHasWrapped)
        {
            return mWriteCursor;
        }
        else
        {
            return Size;
        }
    }

    template<typename ContainedT, size_t Size>
    const ContainedT& CircularBuffer<ContainedT, Size>::PeekNewest(size_t relativeIndex) const
    {
        // Just like in text editing, the most newly written item is just behind
        // the cursor.
        const size_t absoluteIndex{ WrapCursor(mWriteCursor - relativeIndex) };

        if (!IsCursorValid(absoluteIndex))
        {
            // TODO: Return some static "error object"
            return mRing[0];
        }

        return mRing[absoluteIndex];
    }

    template<typename ContainedT, size_t Size>
    const ContainedT& CircularBuffer<ContainedT, Size>::PeekOldest(size_t relativeIndex) const
    {
        const size_t absoluteIndex{ WrapCursor(mWriteCursor + relativeIndex - 1) };

        if (!IsCursorValid(absoluteIndex))
        {
            return mRing[0];
        }

        return *mRing[absoluteIndex];
    }

    template<typename ContainedT, size_t Size>
    CircularBuffer<ContainedT, Size>::ConstIteratorT Kurveball::CircularBuffer<ContainedT, Size>::Begin() const
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


    template<typename ContainedT, size_t Size>
    CircularBuffer<ContainedT, Size>::ConstIteratorT CircularBuffer<ContainedT, Size>::End() const
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

    template<typename ContainedT, size_t Size>
    size_t CircularBuffer<ContainedT, Size>::WrapCursor(size_t absoluteCursor) const
    {
        const size_t wrappedCursor{ absoluteCursor % Size };

        return wrappedCursor;
    }

    template<typename ContainedT, size_t Size>
    bool CircularBuffer<ContainedT, Size>::IsCursorValid(size_t absoluteCursor) const
    {
        const bool isValid{ absoluteCursor < Size };

        // TODO: DebugBreak

        return isValid;
    }
}