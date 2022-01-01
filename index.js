const express = require('express');
const bodyParser = require('body-parser');
const app = express();

const mqtt = require('mqtt')
const host = '192.168.1.5'
const port = '1883'
const clientId = 'local-broker2'
const connectUrl = `mqtt://${host}:${port}`
const client = mqtt.connect(connectUrl, {
    clientId,
    clean: true,
    connectTimeout: 30000,
    reconnectPeriod: 1000,
})

app.get('/', (req, res) => {
    res.send('Server Running');
});

app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: false }))

const topic = 'svgeng/nodemcu/';
app.post("/lamp1/", function (req, res) {
    client.publish(topic+'lamp1', req.body.lamp, { qos: 0, retain: false }, (error) => {
        if (error) {
            console.error(error)
        }
    })

    const onoff = req.body.lamp == 1 ? 'Menyala' : 'Mati';
    res.send({
        "statusCode": 200,
        "message": "Lampu 1 telah "+onoff
    })
});

app.post("/lamp2/", function (req, res) {
    client.publish(topic+'lamp2', req.body.lamp, { qos: 0, retain: false }, (error) => {
        if (error) {
            console.error(error)
        }
    })

    const onoff = req.body.lamp == 1 ? 'Menyala' : 'Mati';
    res.send({
        "statusCode": 200,
        "message": "Lampu 2 telah "+onoff
    })
});

app.post("/lamp3/", function (req, res) {
    client.publish(topic+'lamp3', req.body.lamp, { qos: 0, retain: false }, (error) => {
        if (error) {
            console.error(error)
        }
    })

    const onoff = req.body.lamp == 1 ? 'Menyala' : 'Mati';
    res.send({
        "statusCode": 200,
        "message": "Lampu 3 telah "+onoff
    })
});

app.post("/lamp4/", function (req, res) {
    client.publish(topic+'lamp4', req.body.lamp, { qos: 0, retain: false }, (error) => {
        if (error) {
            console.error(error)
        }
    })

    const onoff = req.body.lamp == 1 ? 'Menyala' : 'Mati';
    res.send({
        "statusCode": 200,
        "message": "Lampu 4 telah "+onoff
    })
});

// client.on('connect', () => {
//     console.log('Connected To MQTT')
// })

app.listen(3000, () => console.log('Server Running at http://localhost:3000'));
