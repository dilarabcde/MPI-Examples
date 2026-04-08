# MPI Examples

This repository contains simple parallel programming examples using MPI (Message Passing Interface) with C++.

The goal is to demonstrate basic communication patterns in distributed systems.

## Examples

### 1. Max Array
Distributed maximum calculation using multiple processes.

### 2. Broadcast Example
Demonstrates how a process sends data to all other processes using `MPI_Bcast`.

## How to Run

Make sure you have MPI installed (e.g., OpenMPI).

Compile:
```bash
mpic++ main.cpp -o program
