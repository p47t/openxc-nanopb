[![Build Status](https://travis-ci.org/p47r1ck7541/openxc-nanopb.svg?branch=master)](https://travis-ci.org/p47r1ck7541/openxc-nanopb)

# What is this?

This is a demonstration showing how to use Protocol Buffer in a Linux kernel module or typically a device driver.

# Why Using Protocol Buffer in Embedded System is a Good Idea?

In embedded system, typically hand-written C struct definitions are used to pass data around even for cross-chip communication. This interface contract between two sides (e.g. device driver and firmware) is too rigid to change. One cannot change the interface without breaking compatibility with the other side. Protocol Buffer can provide versioning mechanism to allow evolving either side independently.

# How can Protocol Buffer be Used in Linux Kernel?

Nanopb is a Protocol Buffer implementation designed for embedded systems. It has small code size and require less than 1KB RAM to run. Besides its dependencies on C runtime library is minimized so that it can be easily ported to be used in Linux kernel. Integrating Nanopb with a kernel module can be done without breaking a sweat.
