const binding = require('./binding')

binding.initialize()
const stream = binding.openOutput(2)
let i = 10
setInterval(() => {
	binding.message(stream, i)
	if (i > 111) {
		binding.terminate()
		Bare.exit()
	}
	i++
}, 100)

