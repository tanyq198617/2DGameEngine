/*
 * Copyright (C) 2012-2016 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "SlotVisitor.h"
#include "Weak.h"
#include "WeakInlines.h"

namespace JSC {

inline void SlotVisitor::appendUnbarriered(JSValue* slot, size_t count)
{
    for (size_t i = count; i--;)
        appendUnbarriered(slot[i]);
}

inline void SlotVisitor::appendUnbarriered(JSCell* cell)
{
    appendUnbarriered(JSValue(cell));
}

template<typename T>
inline void SlotVisitor::append(const Weak<T>& weak)
{
    appendUnbarriered(weak.get());
}

template<typename T>
inline void SlotVisitor::append(const WriteBarrierBase<T>& slot)
{
    appendUnbarriered(slot.get());
}

template<typename T>
inline void SlotVisitor::appendHidden(const WriteBarrierBase<T>& slot)
{
    appendHidden(slot.get());
}

template<typename Iterator>
inline void SlotVisitor::append(Iterator begin, Iterator end)
{
    for (auto it = begin; it != end; ++it)
        append(*it);
}

inline void SlotVisitor::appendValues(const WriteBarrierBase<Unknown>* barriers, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        append(barriers[i]);
}

inline void SlotVisitor::appendValuesHidden(const WriteBarrierBase<Unknown>* barriers, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        appendHidden(barriers[i]);
}

inline void SlotVisitor::reportExtraMemoryVisited(size_t size)
{
    if (!m_isVisitingMutatorStack)
        heap()->reportExtraMemoryVisited(size);
}

#if ENABLE(RESOURCE_USAGE)
inline void SlotVisitor::reportExternalMemoryVisited(size_t size)
{
    if (!m_isVisitingMutatorStack)
        heap()->reportExternalMemoryVisited(size);
}
#endif

inline Heap* SlotVisitor::heap() const
{
    return &m_heap;
}

inline VM& SlotVisitor::vm()
{
    return *m_heap.m_vm;
}

inline const VM& SlotVisitor::vm() const
{
    return *m_heap.m_vm;
}

inline void SlotVisitor::didNotRace(const VisitRaceKey& race)
{
    if (ASSERT_DISABLED)
        return;
    
    if (!m_isVisitingMutatorStack) {
        // This is the first visit so we don't need to remove anything.
        return;
    }
    
    auto locker = holdLock(heap()->m_visitRaceLock);
    heap()->m_visitRaces.remove(race);
}

} // namespace JSC
