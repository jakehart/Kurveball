namespace CurveLib
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
        mHasEverUpdated = true;

        return true; // Success
    }
    
    template<typename ContainedT, size_t Size>
    size_t CircularBuffer<ContainedT, Size>::GetNumContained() const
    {
        if(!mHasEverUpdated)
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