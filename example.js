const binding = require('./binding')

binding.initialize()
const stream = binding.openOutput(3)
let i = 10

setInterval(() => {
  const msg = [240, 0, 32, 41, 2, 24, 10, i, 50, 247]
  binding.writeSysEx(stream, msg)
  if (i > 111) {
    binding.terminate()
    Bare.exit()
  }
  i++
}, 100)
