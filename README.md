# WordFreq

*Parse an input file and return the most frequently used words with their frequency.*

[![Master Build Status](https://secure.travis-ci.org/tecnickcom/wordfreq.png?branch=master)](https://travis-ci.org/tecnickcom/wordfreq?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/tecnickcom/wordfreq/badge.svg?branch=master&service=github)](https://coveralls.io/github/tecnickcom/wordfreq?branch=master)

* **category**    Tools
* **license**     MIT (see LICENSE)
* **author**      Nicola Asuni
* **copyright**   2017-2018 Nicola Asuni - Tecnick.com
* **link**        https://github.com/tecnickcom/wordfreq


## Description

This program parse an input file and return the most frequently used words with their frequency.
In this context a word is a continuous sequence of characters from 'a' to 'z'.
The words are case-insensitive, so uppercase letters are always mapped in lowercase.

The output is similar to that of the following bash command:

```
cat <FILENAME> | tr -cs 'a-zA-Z' '[\n*]' | grep -v "^$" | tr '[:upper:]' '[:lower:]'| sort | uniq -c | sort -nr | head -20
```

## Getting Started

### Development dependencies:

* alien
* astyle
* build-essential
* clang-tidy
* cmake
* debhelper
* devscripts
* dh-make
* doxygen
* fakeroot
* lcov
* make
* pkg-config
* rpm

A wrapper Makefile is available to allows building the project in a Linux-compatible system with simple commands.  
All the artifacts and reports produced using this Makefile are stored in the *target* folder.  

To see all available options:
```
make help
```

Use the command ```make all``` to build and test all.

Use the command ```make dbuild``` to build everything inside a docker container.

The build and test artifacts are inside the `target` folder.
