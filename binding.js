const EventEmitter = require('bare-events')
const binding = require.addon()
const constants = require('./lib/constants.js')

binding.initialize()

class MidiInput extends EventEmitter {
  constructor(id) {
    super()
    this._stream = binding.openInput(id)
    this._sleep = 10
    this._sysex = null
    this._interval = setInterval(this._read.bind(this), this._sleep)
  }

  _read() {
    let data

    try {
      data = binding.readMessage(this._stream)
    } catch (err) {
      this._sysex = null
      this.emit('error', err)
      return
    }

    for (let i = 0; i < data.length; i += 4) {
      this._onEvent(data.slice(i, i + 4))
    }
  }

  _onEvent(bytes) {
    const status = bytes[0]

    if (status >= constants.REALTIME) return this.emit('clock', status)

    if (this._sysex === null) {
      if (status !== constants.SYSEX_START) {
        return this.emit('message', bytes.slice(0, constants.messageLength(status)))
      }

      this._sysex = []
    } else if (status >= 0x80 && status !== constants.SYSEX_END) {
      const partial = this._sysex

      this._sysex = null
      this.emit('truncated', partial)

      return this._onEvent(bytes)
    }

    for (const byte of bytes) {
      this._sysex.push(byte)

      if (byte === constants.SYSEX_END) {
        const sysex = this._sysex

        this._sysex = null
        this.emit('sysex', sysex)

        return
      }
    }
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
