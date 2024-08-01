const express = require('express');
const cors = require('cors');
const app = express();
const port = 3000;

app.use(cors()); // Enable CORS for all routes
app.use(express.json());

let sensors = {
    "0x01": "",
    "0x78": "",
    "0xA0": "",
    "0xF3": "",
    "0xFD": ""
};

app.get('/getSensorList', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(sensors);
});

app.post('/setSensorList', (req, res) => {
    console.log(req.body);
    sensors = req.body;
    res.status(200).send('Data received');
});

app.listen(port, () => {
    console.log(`Mock server running at http://localhost:${port}`);
});
const BASE62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

function encode64BitNumberToBase62(num) {
    let bigIntNum = BigInt(num);
    let base62String = "";

    while (bigIntNum > 0) {
        let remainder = bigIntNum % 62n;
        base62String = BASE62[Number(remainder)] + base62String;
        bigIntNum /= 62n;
    }

    return base62String;
}
let number = "2810246214155019048";
let encodedString = encode64BitNumberToBase62(number);
console.log(encodedString); // Example output: 
