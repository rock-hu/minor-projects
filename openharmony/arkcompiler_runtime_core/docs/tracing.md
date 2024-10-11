# Panda Tracing

This document describes **Panda trace** subsystem. The subsystem provides API for creating *tracepoints* to track key points in the runtime. The subsystem uses the `ftrace` ring buffer to record the trace.

## API
Trace API is described in libpandabase/trace/trace.h file. It supports tracing a scope execution time and tracking a parameter value.
### Usage examples:
```cpp
...
#include "trace/trace.h"
...

void FunctionA() {
    trace::ScopedTrace scoped_trace("Loading file");
    ...
}

void FunctionB() {
    trace::ScopedTrace scoped_trace(__func__);
    ...
}

void FunctionC() {
    SCOPED_TRACE_STREAM << "Trace: " << __func__;
    ...
}

void FunctionD() {
    trace::BeginTracePoint(__func__);
    ...
    trace::EndTracePoint();
}

void FunctionE(int allocated_bytes) {
    trace::IntTracePoint("Heap Size", allocated_bytes);
    ...
}
```

## Recording trace

To record and view a trace, do the following steps:

1. Enable tracing by the command:
```bash
sudo scripts/trace_enable.sh <output_file> <trace_time_in_seconds>
```
2. Launch the runtime with extra environment variable:
```bash
PANDA_TRACE=1 panda <args>
```
3. Stop tracing by ^C if the trace time is still running out.
4. Load <output_file> in Chrome at `chrome://tracing` address.
