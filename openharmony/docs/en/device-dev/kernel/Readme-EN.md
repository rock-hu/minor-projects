# Kernel

- [Kernel Overview](kernel-overview.md)
- Mini-System Kernel (LiteOS-M)
  - [LiteOS-M Overview](kernel-mini-overview.md)
  - Basic Kernel
    - [Interrupt Management](kernel-mini-basic-interrupt.md)
    - [Task Management](kernel-mini-basic-task.md)
    - [Memory Management](kernel-mini-basic-memory.md)
    - Kernel Communication Mechanisms
      - [Event](kernel-mini-basic-ipc-event.md)
      - [Mutex](kernel-mini-basic-ipc-mutex.md)
      - [Queue](kernel-mini-basic-ipc-queue.md)
      - [Semaphore](kernel-mini-basic-ipc-sem.md)
    - [Time Management](kernel-mini-basic-time.md)
    - [Software Timer](kernel-mini-basic-soft.md)
    - [Doubly Linked List](kernel-mini-basic-list.md)
  - Extension Components
    - [C++ Support](kernel-mini-extend-support.md)
    - [CPUP](kernel-mini-extend-cpup.md)
    - [Dynamic Loading](kernel-mini-extend-dynamic-loading.md)
    - [File System](kernel-mini-extend-file.md)
  - Kernel Debugging
    - [Memory Debugging](kernel-mini-memory-debug.md)
    - [Exception Debugging](kernel-mini-memory-exception.md)
    - [Trace](kernel-mini-memory-trace.md)
    - [LMS](kernel-mini-memory-lms.md)
    - [Shell](kernel-mini-debug-shell.md)
  - Appendix
    - [Kernel Coding Specifications](kernel-mini-appx-code.md)
    - [Standard Libraries](kernel-mini-appx-lib.md)
- Small-System Kernel (LiteOS-A)
  - [LiteOS-A Overview](kernel-small-overview.md)
  - Kernel Startup
    - [Startup in Kernel Space](kernel-small-start-kernel.md)
    - [Startup in User Space](kernel-small-start-user.md)
  - Kernel Basics
    - [Interrupt and Exception Handling](kernel-small-basic-interrupt.md)
    - Process Management
      - [Process](kernel-small-basic-process-process.md)
      - [Task](kernel-small-basic-process-thread.md)
      - [Scheduler](kernel-small-basic-process-scheduler.md)
    - Memory Management
      - [Heap Memory Management](kernel-small-basic-memory-heap.md)
      - [Physical Memory Management](kernel-small-basic-memory-physical.md)
      - [Virtual Memory Management](kernel-small-basic-memory-virtual.md)
      - [Virtual-to-Physical Mapping](kernel-small-basic-inner-reflect.md)
    - Kernel Communication Mechanisms
      - [Event](kernel-small-basic-trans-event.md)
      - [Semaphore](kernel-small-basic-trans-semaphore.md)
      - [Mutex](kernel-small-basic-trans-mutex.md)
      - [Queue](kernel-small-basic-trans-queue.md)
      - [RW Lock](kernel-small-basic-trans-rwlock.md)
      - [Futex](kernel-small-basic-trans-user-mutex.md)
      - [Signal](kernel-small-basic-trans-user-signal.md)
    - [Time Management](kernel-small-basic-time.md)
    - [Software Timer](kernel-small-basic-softtimer.md)
    - [Atomic Operation](kernel-small-basic-atomic.md)
  - Extension Components
    - [System Call](kernel-small-bundles-system.md)
    - [Dynamic Loading and Linking](kernel-small-bundles-linking.md)
    - [Virtual Dynamic Shared Object](kernel-small-bundles-share.md)
    - [LiteIPC](kernel-small-bundles-ipc.md)
    - [Container](kernel-small-bundles-container.md)
    - File Systems
      - [Virtual File System](kernel-small-bundles-fs-virtual.md)
      - [Supported File Systems](kernel-small-bundles-fs-support.md)
      - [File System Adaptation](kernel-small-bundles-fs-new.md)
      - [Plimitsfs](kernel-small-plimits.md)
  - Debugging and Tools
    - Shell
      - [Introduction to Shell](kernel-small-debug-shell-overview.md)
      - [Shell Command Development](kernel-small-debug-shell-guide.md)
      - [Shell Command Programming Example](kernel-small-debug-shell-build.md)
      - Shell Command Reference
        - System Commands
          - [cpup](kernel-small-debug-shell-cmd-cpup.md)
          - [date](kernel-small-debug-shell-cmd-date.md)
          - [dmesg](kernel-small-debug-shell-cmd-dmesg.md)
          - [exec](kernel-small-debug-shell-cmd-exec.md)
          - [free](kernel-small-debug-shell-cmd-free.md)
          - [help](kernel-small-debug-shell-cmd-help.md)
          - [hwi](kernel-small-debug-shell-cmd-hwi.md)
          - [kill](kernel-small-debug-shell-cmd-kill.md)
          - [log](kernel-small-debug-shell-cmd-log.md)
          - [memcheck](kernel-small-debug-shell-cmd-memcheck.md)
          - [oom](kernel-small-debug-shell-cmd-oom.md)
          - [pmm](kernel-small-debug-shell-cmd-pmm.md)
          - [reset](kernel-small-debug-shell-cmd-reset.md)
          - [sem](kernel-small-debug-shell-cmd-sem.md)
          - [stack](kernel-small-debug-shell-cmd-stack.md)
          - [su](kernel-small-debug-shell-cmd-su.md)
          - [swtmr](kernel-small-debug-shell-cmd-swtmr.md)
          - [systeminfo](kernel-small-debug-shell-cmd-sysinfo.md)
          - [task](kernel-small-debug-shell-cmd-task.md)
          - [uname](kernel-small-debug-shell-cmd-uname.md)
          - [vmm](kernel-small-debug-shell-cmd-vmm.md)
          - [watch](kernel-small-debug-shell-cmd-watch.md)
          - [reboot](kernel-small-debug-shell-cmd-reboot.md)
          - [top](kernel-small-debug-shell-cmd-top.md)
        - File Commands
          - [cat](kernel-small-debug-shell-file-cat.md)
          - [cd](kernel-small-debug-shell-file-cd.md)
          - [chgrp](kernel-small-debug-shell-file-chgrp.md)
          - [chmod](kernel-small-debug-shell-file-chmod.md)
          - [chown](kernel-small-debug-shell-file-chown.md)
          - [cp](kernel-small-debug-shell-file-cp.md)
          - [format](kernel-small-debug-shell-file-format.md)
          - [ls](kernel-small-debug-shell-file-ls.md)
          - [lsfd](kernel-small-debug-shell-file-lsfd.md)
          - [mkdir](kernel-small-debug-shell-file-mkdir.md)
          - [mount](kernel-small-debug-shell-file-mount.md)
          - [partinfo](kernel-small-debug-shell-file-partinfo.md)
          - [partition](kernel-small-debug-shell-file-partition.md)
          - [pwd](kernel-small-debug-shell-file-pwd.md)
          - [rm](kernel-small-debug-shell-file-rm.md)
          - [rmdir](kernel-small-debug-shell-file-rmdir.md)
          - [statfs](kernel-small-debug-shell-file-statfs.md)
          - [sync](kernel-small-debug-shell-file-sync.md)
          - [touch](kernel-small-debug-shell-file-touch.md)
          - [writeproc](kernel-small-debug-shell-file-write.md)
          - [umount](kernel-small-debug-shell-file-umount.md)
          - [du](kernel-small-debug-shell-file-du.md)
          - [mv](kernel-small-debug-shell-file-mv.md)
        - Network Commands
          - [arp](kernel-small-debug-shell-net-arp.md)
          - [dhclient](kernel-small-debug-shell-net-dhclient.md)
          - [ifconfig](kernel-small-debug-shell-net-ifconfig.md)
          - [ipdebug](kernel-small-debug-shell-net-ipdebug.md)
          - [netstat](kernel-small-debug-shell-net-netstat.md)
          - [ntpdate](kernel-small-debug-shell-net-ntpdate.md)
          - [ping](kernel-small-debug-shell-net-ping.md)
          - [ping6](kernel-small-debug-shell-net-ping6.md)
          - [telnet](kernel-small-debug-shell-net-telnet.md)
          - [tftp](kernel-small-debug-shell-net-tftp.md)
        - [Magic Key](kernel-small-debug-shell-magickey.md)
        - [User-Space Exception Information](kernel-small-debug-shell-error.md)
    - [Trace](kernel-small-debug-trace.md)
    - [Perf](kernel-small-debug-perf.md)
    - [LMS](kernel-small-memory-lms.md)
    - [Process Debugging](kernel-small-debug-process-cpu.md)
    - Kernel-Mode Memory Debugging
      - [Memory Information Statistics](kernel-small-debug-memory-info.md)
      - [Memory Leak Check](kernel-small-debug-memory-leak.md)
      - [Memory Corruption Check](kernel-small-debug-memory-corrupt.md)
    - [User-Mode Memory Debugging](kernel-small-debug-user.md)
    - Other Kernel Debugging Methods
      - [Dying Gasp](kernel-small-debug-trace-other-lastwords.md)
      - [Common Fault Locating Methods](kernel-small-debug-trace-other-faqs.md)
    - Appendix
      - Basic Data Structure
        - [Doubly Linked List](kernel-small-apx-dll.md)
        - [Bitwise Operation](kernel-small-apx-bitwise.md)
      - [Standard Library](kernel-small-apx-library.md)
      - [Kernel Coding Specification](kernel-mini-appx-code.md)
- Standard-System Kernel (Linux)
  - [Linux Kernel Overview](kernel-standard-overview.md)
  - [Applying Patches on Development Boards](kernel-standard-patch.md)
  - [Compiling and Building the Linux Kernel](kernel-standard-build.md)
  - [HCK](kernel-standard-hck.md)
  - Enhanced Kernel Features
    - [Enhanced Swap](kernel-standard-mm-eswap.md)
    - [New IP Kernel Protocol Stack](kernel-standard-newip.md)
    - Task Scheduling
      - [Related Thread Group](kernel-standard-sched-rtg.md)
      - [Lightweight CPU Isolation](kernel-standard-sched-cpuisolation.md)
