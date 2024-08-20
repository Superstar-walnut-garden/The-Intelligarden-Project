const express = require('express');
const cors = require('cors');
const app = express();
const port = 3000;

app.use(cors()); // Enable CORS for all routes
app.use(express.json());

let sensors = {
    "0x01": "NamedSensor1",
    "0x78": "",
    "0xA0": "",
    "0xF3": "",
    "0xFD": ""
};

let scheduleData = [
    {
        "id": "0",
        "start": "06:09",
        "duration": "05:30",
        "weekday": "1010000",
        "enabled": false,
        "on": false
    },
    {
        "id": "1",
        "start": "13:31",
        "duration": "01:00",
        "weekday": "0110000",
        "enabled": true,
        "on": false
    },
    {
        "id": "2",
        "start": "08:00",
        "duration": "01:00",
        "weekday": "0010000",
        "enabled": true,
        "on": false
    },
    {
        "id": "3",
        "start": "07:30",
        "duration": "01:00",
        "weekday": "0010100",
        "enabled": true,
        "on": false
    },
    {
        "id": "4",
        "start": "12:30",
        "duration": "07:00",
        "weekday": "0000100",
        "enabled": false,
        "on": false
    },
    {
        "id": "5",
        "start": "05:09",
        "duration": "06:00",
        "weekday": "0101000",
        "enabled": false,
        "on": true
    },
    {
        "id": "6",
        "start": "13:09",
        "duration": "04:00",
        "weekday": "1100000",
        "enabled": false,
        "on": false
    }
];
let currentTimeData = {
    "time": "6:45",
    "weekday": "1000000"
}
app.get('/getCurrentTime', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(currentTimeData);
    console.log("getCurrentTime request handled!");
});

app.get('/getPumpSchedule', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(scheduleData);
    console.log("getPumpSchedule request handled!");
});
app.post('/setPumpSchedule', (req, res) => {
    scheduleData = req.body;
    console.log("setPumpSchedule request handled!");
    res.setHeader('Content-Type', 'text/plain');
    res.status(200).send('');
});

app.get('/getSensorList', (req, res) => {
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
