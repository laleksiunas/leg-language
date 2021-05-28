# Leg language

An example language, created using LLVM, which comes with built-in parallelism 
support and Python-like syntax. This project is supposed to give a head start for
LLVM newcomers with examples of how to implement custom types system, block scopes,
functions overloading and C functions calls.

## Syntax

```
fn longTask(end: int): int
  sleep(2)
  
  let result = 1
  
  for (i in 2..end)
    result *= i
  
  return result

fn main(): void
  println("Hello world")
  
  // run function calls in parallel. main function takes 2s to execute
  // due to sleep(2) call in the longTask function
  a, b = catch {
    kick longTask(5),
    kick longTask(8)
  }
  
  print("5! = ")
  println(a)
  print("8! = ")
  println(b)
```
More examples of the language syntax can be found in the _examples_ directory.

## Prerequisites
* CMake (v3.17) & Make (v3.81) build tools
* Flex & Bison for grammar parsing
* LLVM (v12.0.0)
* Conan (v1.35.1) C/C++ package manager

## Building
1. Run _install.sh_ script to install Conan dependencies
2. Run _compile.sh_ script to compile the project
    * `sh compile.sh` for Debug build
    * `sh compile.sh Release` for Release build

## Running
* Run an example via _run-example.sh_ script:
  * `sh run-example.sh hello-world.leg` using Debug build
  * `sh run-example.sh hello-world.leg Release` using Release build
* Run a specific file while using cli executable:
  `cmake-build-{type}/bundle/leg_cli file.leg`

## Tested platforms
* macOS 11.3.1 x86-64
