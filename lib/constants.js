const SYSEX_START = 0xf0
const SYSEX_END = 0xf7
const REALTIME = 0xf8

const CHANNEL_LENGTHS = {
  0x80: 3, // note off
  0x90: 3, // note on
  0xa0: 3, // polyphonic aftertouch
  0xb0: 3, // control change
  0xc0: 2, // program change
  0xd0: 2, // channel aftertouch
  0xe0: 3 // pitch bend
}

const SYSTEM_LENGTHS = {
  0xf1: 2, // time code quarter frame
  0xf2: 3, // song position pointer
  0xf3: 2, // song select
  0xf4: 1, // undefined
  0xf5: 1, // undefined
  0xf6: 1, // tune request
  0xf7: 1 // end of sysex, only reached when stray
}

function messageLength(status) {
  if (status < SYSEX_START) return CHANNEL_LENGTHS[status & 0xf0]
  return SYSTEM_LENGTHS[status]
}

module.exports = { SYSEX_START, SYSEX_END, REALTIME, CHANNEL_LENGTHS, messageLength }

