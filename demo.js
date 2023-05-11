#!/usr/local/bin/atom
"use strict";
"use atom"; // aaaa
"use compiler";
import * as std from "std";
function main(argc, argv) {
  std.printf(`aaa${argc + { a: 123 }}bbb${123 + 456}ccc`); /* template */
  a = 2
  return 0;
}
class demo {
  #data = 123.123;
  constructor() {
    this.#data++;
  }
}
