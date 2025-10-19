const binding = require('./binding')

binding.initialize()
console.log('count devices', binding.countDevices())
binding.terminate()
