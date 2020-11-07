const serial = require("serialport")
let chars = []

const port = new serial("COM3", { baudRate: 115200 }, (e) => {
    if(e) {
        console.error(e)
    }
})
port.on('readable', function() {
    chars.push(...port.read());
    printChars();
})

function printChars() {
    const newLine = chars.indexOf(10);
    if(newLine != -1) {
        const line = chars.splice(0, newLine + 1);
        let lineStr = "";
        for(let i = 0; i < line.length - 1; i++) {
            lineStr += String.fromCharCode(line[i]);
        }
        console.log(lineStr)
        printChars();
    }
}
port.on('error', function(err) {
    console.error('Error: ', err.message)
})

globalThis.write = (line) => {
    port.write(line)
}

spamE = false;

globalThis.spam = (line) => {
    let amount = 0;
    if(spamE) {
        spamE = false;
    } else {
        spamE = true;
    }
    const interval = setInterval(() => {
        if(!spamE) {
            clearInterval(interval);
        }
        port.write("1");
        amount++;
        if(amount % 100 == 0) {
            console.log('sending')
        }
    }, 10);

}