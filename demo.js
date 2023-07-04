function print(fmt, ...args) {
  const time = Date.now();
  console.log(fmt, time, ...args);
  return args.length;
}
