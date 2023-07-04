class A {
  @a() @blur()
  static async *log(fmt, arg) {
    print(fmt, arg);
  }
  #a = 2;
}
