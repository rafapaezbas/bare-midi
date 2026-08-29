const EventEmitter = require('bare-events')
const binding = require.addon()

binding.initialize()

class MidiInput extends EventEmitter {
  constructor(id) {
    super()
    this._stream = binding.openInput(id)
    this._sleep = 10
    this._interval = setInterval(() => {
      const message = binding.readMessage(this._stream)
      if (message[0] === 0 && message[1] === 0 && message[2] === 0) return // no new message
      this.emit('message', message)
    }, this._sleep)
  }
}

class MidiOutput {
  constructor(id) {
    this._stream = binding.openOutput(id)
  }

  writeSysEx(msg) {
    binding.writeSysEx(this._stream, msg)
  }
}

module.exports = { MidiInput, MidiOutput }
