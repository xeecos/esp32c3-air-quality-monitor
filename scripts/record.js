const fs = require("fs");
const { SerialPort } = require('serialport');
const args = require('minimist')(process.argv.slice(2))
if(args['port'])
{
    const during = (args['during'] || 10) * 1000;
    const port = new SerialPort({ path: args['port'], baudRate: 115200, autoOpen: false });
    port.open(function (err) {
        if (err) {
            return console.log('Error opening port: ', err.message)
        }
    })
    let buf = "";
    let time = 0;
    let fileName = args['name'] || new Date().toLocaleString().split("/").join("-").split(" ").join("_").split(":").join("_");
    console.log("recording:",fileName);
    fs.writeFileSync(`${fileName}.csv`,`time|pm1.0|pm2.5|pm10\n`);
    port.on('data', function (data) {
        buf += data.toString();
        if(buf.indexOf('\n') != -1)
        {
            let lines = buf.split('\n');
            buf = lines[lines.length - 1];
            lines.pop();
            lines.forEach(line => {
                if(Date.now()-time>during)
                {
                    time = Date.now();
                    fs.appendFileSync(`${fileName}.csv`,`${time}|${line.split(" ").map(item=>{return item.split(":")[1];}).join("|")}\n`);
                }
            });
        }
    })
}
else
{
    console.log("example: node record.js --port=COM5 --during=10 --name=test")
}