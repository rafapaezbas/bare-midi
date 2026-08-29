const { MidiInput, MidiOutput } = require('./binding')

const input = new MidiInput(1)
const output = new MidiOutput(3)
input.on('message', (message) => console.log('new message', message))

let i = 10

setInterval(() => {
  const msg = [240, 0, 32, 41, 2, 24, 10, i, 50, 247]
  output.writeSysEx(msg)
  if (i > 111) {
    Bare.exit()
  }
  i++
}, 100)
