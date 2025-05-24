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
        "id": 0,
        "event_id": -1,
        "name": "pump schedule",
        "start": "18:00",
        "duration": "01:15",
        "weekday": "1111111",
        "enabled": true,
        "on": true
        
    },
    {
        "id": 1,
        "event_id": -1,
        "name": "Fan schedule",
        "start": "18:00",
        "duration": "01:15",
        "weekday": "1111111",
        "enabled": true,
        "on": true
    }
];

let eventList = [
    {
        "id": 0,
        "name": "pump event",
        "occupied": false,
        "on": false
    },
    {
        "id": 1,
        "name": "Lights event",
        "occupied": false,
        "on": false
    },
    {
        "id": 2,
        "name": "Fan event",
        "occupied": false,
        "on": false
    },
    {
        "id": 3,
        "name": "valve event",
        "occupied": false,
        "on": false
    }
];

let pinList = [
    {
        "id": 16,
        "name": "valve pin",
        "status": false,
        "mode": 1,
        "event_id": 3
    },
    {
        "id": 20,
        "name": "Lamp pin",
        "status": false,
        "mode": 1,
        "event_id": 1
    },
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

app.get('/getScheduleList', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(scheduleData);
    console.log("getPumpSchedule request handled!");
});

app.post('/createSchedule', (req, res) => {
    const newSchedule = req.body;
    scheduleData.push(newSchedule);
    res.status(200).send('Schedule created');
});

app.post('/deleteSchedule', (req, res) => {
    const { id } = req.body;
    scheduleData = scheduleData.filter(schedule => schedule.id !== id);
    res.status(200).send('Schedule deleted');
});

app.post('/modifySchedule', (req, res) => {
    const updatedSchedule = req.body;
    scheduleData = scheduleData.map(schedule => schedule.id === updatedSchedule.id ? updatedSchedule : schedule);
    res.status(200).send('Schedule modified');
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

app.get('/getEventList', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(eventList);
    console.log("getEventList request handled!");
});

app.post('/createEvent', (req, res) => {
    const newEvent = req.body;
    eventList.push(newEvent);
    res.status(200).send('Event created');
});

app.post('/deleteEvent', (req, res) => {
    const { id } = req.body;
    eventList = eventList.filter(event => event.id !== id);
    res.status(200).send('Event deleted');
});

app.post('/modifyEvent', (req, res) => {
    const updatedSchedule = req.body;
    eventList = eventList.map(event => event.id === updatedEvent.id ? updatedEvent : event);
    res.status(200).send('Event modified');
});

app.get('/getGPIOList', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(pinList);
    console.log("getGPIOList request handled!");
});

app.post('/createGPIO', (req, res) => {
    const newPin = req.body;
    pinList.push(newPin);
    res.status(200).send('GPIO created');
});

app.post('/deleteGPIO', (req, res) => {
    const { id } = req.body;
    pinList = pinList.filter(pin => pin.id !== id);
    res.status(200).send('GPIO deleted');
});

app.post('/modifyGPIO', (req, res) => {
    const updatedGPIO = req.body;
    pinList = pinList.map(pin => pin.id === updatedGPIO.id ? updatedGPIO : pin);
    res.status(200).send('GPIO modified');
});

app.listen(port, () => {
    console.log(`Mock server running at http://localhost:${port}`);
});
