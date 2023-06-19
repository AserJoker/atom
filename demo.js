a = {
  novalue,
  ...rest,
  key: value,
  ["compute"]: value,
  ["calc"](a, b) {
    a+b
  },
  async *update_value(d) {
    d++
  },
  log: (val) => {
    console.log(val)
  }
};
