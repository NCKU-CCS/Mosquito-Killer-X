var request = require('request');

var counter = 0;

var SerialPort = require("serialport").SerialPort
var serialPort = new SerialPort("/dev/ttyS0", {
  baudrate: 9600
});


var json_config = {
	"machine_data":"netdb's linkit",
};

serialPort.on("open", function () {
  serialPort.on('data', function (data) {
	
	console.log(date);
	formData = {
		"bugZapperId": "143",
		"lng": 120.229160,
		"lat": 23.997916,
        "cnt": parseInt(data)
	}


    var url = 'http://52.196.80.43:3000/bugZapper/'
    var options = {
        method: 'post',
        body: formData,
        json: true,
        url: url
    }
    request(options, function (err, res, body) {
        if (err) {
            console.error('error posting json: ', err)
            throw err
        }
        var headers = res.headers
        var statusCode = res.statusCode
        console.log('headers: ', headers)
        console.log('statusCode: ', statusCode)
        console.log('body: ', body)
    })
//	request.post('http://52.196.80.43:3000/bugZapper/', payload);
  });
});
