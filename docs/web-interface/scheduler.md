# How pump.html client communicate with server (esp32 node)?

### client to server json data:
```
[
    {
        "id": "0",
        "start": "18:00",
        "duration": "01:15",
        "weekday": "1111111",
        "enabled": true,
        "on": true
    },
    {
        "id": "1",
        "start": "08:00",
        "duration": "01:15",
        "weekday": "1111111",
        "enabled": true,
        "on": true
    }
]
```
### server to client json data:
```
[
    {
        "id": "0",
        "start": "18:00",
        "duration": "01:15",
        "weekday": "1111111",
        "enabled": true,
        "on": true
    },
    {
        "id": "1",
        "start": "08:00",
        "duration": "01:15",
        "weekday": "1111111",
        "enabled": true,
        "on": true
    }
]
+
{
    "current-time" = "14:30",
    "current-date" = "1/1/2024"
}
```