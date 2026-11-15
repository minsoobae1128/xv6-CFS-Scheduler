# xv6 CFS-Inspired CPU Scheduler

A modified **xv6-public (x86)** operating-system project that replaces the original round-robin process selection with a simplified scheduler based on the ideas of Linux's Completely Fair Scheduler (CFS).

This implementation was developed for **SWE3004 Operating Systems - Project 2: CPU Scheduling**. It is a course-project implementation of selected CFS concepts, not a complete implementation of the Linux kernel scheduler.

## Overview

The scheduler chooses the runnable process with the smallest virtual runtime (`vruntime`). Each process receives a time slice proportional to its scheduling weight, which is derived from its `nice` value. Runtime statistics are updated on timer interrupts, and the `ps()` system call is extended to display scheduling information.

## Implemented Features

- **Minimum-`vruntime` scheduling:** Scan the process table and select the `RUNNABLE` process with the smallest `vruntime`.
- **Nice-based weights:** Support `nice` values from 0 to 39, with a default of 20. A predefined weight table is used, and the weight for `nice = 20` is 1024.
- **Proportional time slices:** Allocate a time slice using the project's 10-tick scheduling latency:

  ```text
  time_slice = 10 * process_weight / total_runnable_weight
  ```

  Integer arithmetic is used, with a minimum time slice of one tick to avoid a zero-length slice.

- **Runtime accounting:** Increase a running process's `runtime` by 1000 for each timer tick (millitick units).
- **Virtual-runtime accounting:** Increase `vruntime` on each timer tick according to the process weight:

  ```text
  vruntime += (1000 * 1024) / process_weight
  ```

- **Time-slice expiration:** Track ticks consumed by the current process and yield the CPU when `ticks_used` reaches its assigned time slice.
- **Fork inheritance:** A newly forked process inherits its parent's `runtime`, `vruntime`, and `nice` value.
- **Wake-up handling:** Set a waking process's `vruntime` relative to the minimum `vruntime` among runnable processes, subtracting the waking process's one-tick virtual-runtime increment. If no process is runnable, set the waking process's `vruntime` to zero. The wake-up path does not call `sched()`.
- **Process monitoring:** Extend `ps()` to show process name, PID, state, `nice`, `runtime/weight`, `runtime`, `vruntime`, and total tick. Scheduling statistics are displayed using integer millitick values rather than floating-point numbers.

## Scheduling Flow

1. Acquire the process-table lock.
2. Scan runnable processes, calculate their weights, and find the process with the smallest `vruntime`.
3. Calculate the selected process's time slice from its weight and the total runnable weight.
4. Reset its `ticks_used` counter and switch to the selected process.
5. On each timer interrupt, update `runtime`, `vruntime`, and `ticks_used` for the running process.
6. Yield when the process has consumed its time slice, then repeat the selection process.

## Main Code Locations

| File | Responsibility |
|---|---|
| `proc.h` | Stores per-process scheduling fields. |
| `proc.c` | Implements the scheduler, process initialization and fork inheritance, wake-up handling, and process-statistics output. |
| `trap.c` | Updates runtime statistics on timer interrupts and enforces time-slice expiration. |
| `sysproc.c`, `syscall.c`, `syscall.h`, `user.h`, `usys.S` | Connect process-management interfaces to user programs. |
| `mytest.c` | Checks process-priority interfaces, invalid inputs, and `ps()` output. |
| `cfstest.c` | Runs competing CPU-bound processes with different `nice` values and compares their scheduling statistics. |
| `Makefile` | Builds the kernel and includes the test programs in the xv6 file system image. |

## Build and Run

The project uses the x86 version of xv6 and QEMU. Build it in a Linux environment with a compatible x86 GCC/binutils toolchain and QEMU installed.

```bash
make clean
make qemu-nox
```

After xv6 boots, run the included test programs from the xv6 shell:

```text
$ mytest
$ cfstest
```

`mytest` checks the process-priority interfaces and process information output. `cfstest` runs two CPU-bound child processes with different `nice` values and prints their scheduling statistics.

## Test Results

The following values are from one run of `cfstest`, as recorded in the project report:

| Nice | Weight | Runtime (milliticks) | Vruntime (milliticks) |
|---:|---:|---:|---:|
| 10 | 9,548 | 81,000 | 8,239 |
| 15 | 3,121 | 29,000 | 8,872 |

In this run, the `nice = 10` process received more CPU time than the `nice = 15` process. The runtime ratio was approximately **2.79**, while the weight ratio was approximately **3.06**. The two virtual-runtime values remained relatively close. This is consistent with the intended priority-sensitive behavior, but the values are from a short test run and should not be interpreted as a benchmark or an exact proportionality guarantee.

The report also records checks for successful and invalid `setnice()` inputs, `getnice()` with an invalid PID, and `ps()` output for all processes and a specified PID.

## Scope and Limitations

- This is a simplified educational implementation of CFS concepts in xv6. It does not reproduce all mechanisms of the Linux scheduler.
- The scheduler scans the xv6 process table to find the minimum `vruntime`; it does not use Linux's scheduler data structures.
- The project report documents functional checks and a competing-process test. It does **not** include a dedicated stress test focused on repeated sleep/wakeup transitions.
- The observed scheduling ratios depend on the test duration, workload, and integer time-slice calculations.

## Project Report

The detailed report describes the design, modified kernel components, code excerpts, test procedures, and observed results:

- Project2_CFS_Report.docx

## Acknowledgements

This project is based on the x86 version of **xv6-public**. The original xv6 source and its licensing notices are retained in this repository. See [`LICENSE`](LICENSE) and the [MIT PDOS xv6-public repository](https://github.com/mit-pdos/xv6-public) for upstream information.
