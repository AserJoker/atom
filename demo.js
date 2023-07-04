class A {
  @a()
  @blur()
  static async *log(fmt, arg) {
    test: print(fmt, arg);
    yield 1;
    print(fmt, arg, arg);
    return 0;
  }
  #a = 2;
}