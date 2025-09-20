SDRuno Plugin Threading Architecture Fix
==========================================

PROBLEM SOLVED:
• Settings crash due to cross-thread GUI access and form type issues
• Main UI not showing metrics (cross-thread update problems)
• Plugin not shutting down cleanly

ARCHITECTURE BEFORE (❌ PROBLEMATIC):
┌─────────────────────┐
│   Main Thread       │  StreamObserverProcess()
│   SDR Processing    │────────────────────────┐
└─────────────────────┘                       │
                                              │ DIRECT UI CALL
                                              ▼ (CRASH RISK)
┌─────────────────────┐                ┌─────────────────────┐
│   UI Thread         │                │   Settings Dialog   │
│   nana::exec()      │                │   nana::form        │
│   (BLOCKED!)        │                │   (CRASHES!)        │
└─────────────────────┘                └─────────────────────┘

ARCHITECTURE AFTER (✅ FIXED):
┌─────────────────────┐
│   Stream Thread     │  StreamObserverProcess()
│   SDR Processing    │  Calculate RC, INR, LF, RDE
└─────────────────────┘
           │
           │ Thread-Safe Call
           │ affinity_execute()
           ▼
┌─────────────────────┐
│   GUI Thread        │  nana::exec() Event Loop
│   Dedicated Thread  │  ┌─────────────────────┐
│                     │  │ Main Form           │
│                     │  │ UpdateMetrics()     │
│                     │  └─────────────────────┘
│                     │  ┌─────────────────────┐
│                     │  │ Settings Dialog     │
│                     │  │ nested_form         │
│                     │  │ (Anchored & Safe)   │
│                     │  └─────────────────────┘
└─────────────────────┘
           ▲
           │ HandleEvent(ClosingDown)
           │ Clean Shutdown
┌─────────────────────┐
│   Plugin Lifecycle  │  Unregister observers
│   Event Handling    │  Close UI safely
└─────────────────────┘

KEY CHANGES:
1. ✅ Dedicated GUI thread runs nana::exec()
2. ✅ Thread-safe UI updates via affinity_execute() 
3. ✅ Settings as nested_form (prevents crashes)
4. ✅ HandleEvent in main plugin for lifecycle
5. ✅ Proper shutdown sequence: UI → observers → cleanup

RESULT:
✅ Settings dialog opens without crashes
✅ Real-time metrics display works
✅ Clean plugin shutdown
✅ No cross-thread UI violations