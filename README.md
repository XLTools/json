JsonReader++
============

JsonReader++ is a memory-efficient, lazy C++11 JSON reader and writer.

**Table of Contents**

- [Motivation](#motivation)

## Motivation

Most parsers provide one of two interfaces: a DOM API, which parses the entire document tree, and a SAX API, which provides event-based handling for lazy parsing. Although DOM parsers are easy-to-use, they scale poorly with large documents. SAX parsers, on the other hand, require large amounts of boilerplate for basic tasks, making it difficult to do complex processing.

The .NET library took a different approach, and designed push/pull-based APIs with a TextReader and TextWriter approach: the API iterates over nodes in the document, allowing you to process data at each step. JsonReader++

## Requirements

JSON has no external dependencies, and only requires a C++11 compiler. It has been tested on
