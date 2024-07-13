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
