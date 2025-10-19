const binding = require('./binding')

binding.initialize()
console.log('count devices', binding.countDevices())
console.log('device info 0:', binding.getDeviceInfo(0))
binding.terminate()
