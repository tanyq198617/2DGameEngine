/*
 * Copyright (C) 2011-2017 Apple Inc. All rights reserved.
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

#include "CellState.h"
#include "HandleTypes.h"
#include "MarkStack.h"
#include "OpaqueRootSet.h"
#include "VisitRaceKey.h"
#include <wtf/MonotonicTime.h>

namespace JSC {

class ConservativeRoots;
class GCThreadSharedData;
class Heap;
class HeapCell;
class HeapSnapshotBuilder;
class MarkedBlock;
class UnconditionalFinalizer;
template<typename T> class Weak;
class WeakReferenceHarvester;
template<typename T> class WriteBarrierBase;

typedef uint32_t HeapVersion;

class SlotVisitor {
    WTF_MAKE_NONCOPYABLE(SlotVisitor);
    WTF_MAKE_FAST_ALLOCATED;

    friend class SetCurrentCellScope;
    friend class Heap;

public:
    SlotVisitor(Heap&);
    ~SlotVisitor();

    MarkStackArray& collectorMarkStack() { return m_collectorStack; }
    MarkStackArray& mutatorMarkStack() { return m_mutatorStack; }
    const MarkStackArray& collectorMarkStack() const { return m_collectorStack; }
    const MarkStackArray& mutatorMarkStack() const { return m_mutatorStack; }
    
    VM& vm();
    const VM& vm() const;
    Heap* heap() const;

    void append(ConservativeRoots&);
    
    template<typename T> void append(const WriteBarrierBase<T>&);
    template<typename T> void appendHidden(const WriteBarrierBase<T>&);
    template<typename Iterator> void append(Iterator begin , Iterator end);
    void appendValues(const WriteBarrierBase<Unknown>*, size_t count);
    void appendValuesHidden(const WriteBarrierBase<Unknown>*, size_t count);
    
    // These don't require you to prove that you have a WriteBarrier<>. That makes sense
    // for:
    //
    // - roots.
    // - sophisticated data structures that barrier through other means (like DFG::Plan and
    //   friends).
    //
    // If you are not a root and you don't know what kind of barrier you have, then you
    // shouldn't call these methods.
    JS_EXPORT_PRIVATE void appendUnbarriered(JSValue);
    void appendUnbarriered(JSValue*, size_t);
    void appendUnbarriered(JSCell*);
    
    template<typename T>
    void append(const Weak<T>& weak);
    
    JS_EXPORT_PRIVATE void addOpaqueRoot(void*);
    JS_EXPORT_PRIVATE bool containsOpaqueRoot(void*) const;
    TriState containsOpaqueRootTriState(void*) const;

    bool isEmpty() { return m_collectorStack.isEmpty() && m_mutatorStack.isEmpty(); }

    void didStartMarking();
    void reset();
    void clearMarkStacks();

    size_t bytesVisited() const { return m_bytesVisited; }
    size_t visitCount() const { return m_visitCount; }
    
    void addToVisitCount(size_t value) { m_visitCount += value; }

    void donate();
    void drain(MonotonicTime timeout = MonotonicTime::infinity());
    void donateAndDrain(MonotonicTime timeout = MonotonicTime::infinity());
    
    enum SharedDrainMode { SlaveDrain, MasterDrain };
    enum class SharedDrainResult { Done, TimedOut };
    SharedDrainResult drainFromShared(SharedDrainMode, MonotonicTime timeout = MonotonicTime::infinity());

    SharedDrainResult drainInParallel(MonotonicTime timeout = MonotonicTime::infinity());
    SharedDrainResult drainInParallelPassively(MonotonicTime timeout = MonotonicTime::infinity());

    // This informs the GC about auxiliary of some size that we are keeping alive. If you don't do
    // this then the space will be freed at end of GC.
    void markAuxiliary(const void* base);

    void reportExtraMemoryVisited(size_t);
#if ENABLE(RESOURCE_USAGE)
    void reportExternalMemoryVisited(size_t);
#endif
    
    void addWeakReferenceHarvester(WeakReferenceHarvester*);
    void addUnconditionalFinalizer(UnconditionalFinalizer*);

    void dump(PrintStream&) const;

    bool isBuildingHeapSnapshot() const { return !!m_heapSnapshotBuilder; }
    
    HeapVersion markingVersion() const { return m_markingVersion; }

    void mergeOpaqueRootsIfNecessary();
    
    bool mutatorIsStopped() const { return m_mutatorIsStopped; }
    
    Lock& rightToRun() { return m_rightToRun; }
    
    void updateMutatorIsStopped(const AbstractLocker&);
    void updateMutatorIsStopped();
    
    bool hasAcknowledgedThatTheMutatorIsResumed() const;
    bool mutatorIsStoppedIsUpToDate() const;
    
    void optimizeForStoppedMutator();
    
    void didRace(const VisitRaceKey&);
    void didRace(JSCell* cell, const char* reason) { didRace(VisitRaceKey(cell, reason)); }
    
    void visitAsConstraint(const JSCell*);
    
    bool didReachTermination();

private:
    friend class ParallelModeEnabler;
    
    void appendJSCellOrAuxiliary(HeapCell*);
    void appendHidden(JSValue);

    JS_EXPORT_PRIVATE void setMarkedAndAppendToMarkStack(JSCell*);
    
    template<typename ContainerType>
    void setMarkedAndAppendToMarkStack(ContainerType&, JSCell*);
    
    void appendToMarkStack(JSCell*);
    
    template<typename ContainerType>
    void appendToMarkStack(ContainerType&, JSCell*);
    
    void appendToMutatorMarkStack(const JSCell*);
    
    void noteLiveAuxiliaryCell(HeapCell*);
    
    JS_EXPORT_PRIVATE void mergeOpaqueRoots();
    void mergeOpaqueRootsIfProfitable();

    void visitChildren(const JSCell*);
    
    void donateKnownParallel();
    void donateKnownParallel(MarkStackArray& from, MarkStackArray& to);
    
    bool hasWork(const LockHolder&);
    bool didReachTermination(const LockHolder&);

    MarkStackArray m_collectorStack;
    MarkStackArray m_mutatorStack;
    OpaqueRootSet m_opaqueRoots; // Handle-owning data structures not visible to the garbage collector.
    
    size_t m_bytesVisited;
    size_t m_visitCount;
    bool m_isInParallelMode;
    
    HeapVersion m_markingVersion;
    
    Heap& m_heap;

    HeapSnapshotBuilder* m_heapSnapshotBuilder { nullptr };
    JSCell* m_currentCell { nullptr };
    bool m_isVisitingMutatorStack { false };
    bool m_mutatorIsStopped { false };
    bool m_canOptimizeForStoppedMutator { false };
    Lock m_rightToRun;
    
public:
#if !ASSERT_DISABLED
    bool m_isCheckingForDefaultMarkViolation;
    bool m_isDraining;
#endif
};

class ParallelModeEnabler {
public:
    ParallelModeEnabler(SlotVisitor& stack)
        : m_stack(stack)
    {
        ASSERT(!m_stack.m_isInParallelMode);
        m_stack.m_isInParallelMode = true;
    }
    
    ~ParallelModeEnabler()
    {
        ASSERT(m_stack.m_isInParallelMode);
        m_stack.m_isInParallelMode = false;
    }
    
private:
    SlotVisitor& m_stack;
};

} // namespace JSC
