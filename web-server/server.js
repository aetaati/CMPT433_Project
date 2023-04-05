const express = require('express');
const fileUpload = require('express-fileupload');
const fs = require('fs');
var path = require('path')
const app = express();

const cors = require('cors')

const ffmpegPath = require('@ffmpeg-installer/ffmpeg').path;
const ffmpeg = require('fluent-ffmpeg');
ffmpeg.setFfmpegPath(ffmpegPath);

app.use(cors());
app.use(fileUpload());

var dgram = require('dgram');

function sendUDP(data) {
    // Info for connecting to the local process via UDP
    var PORT = 12345;
    var HOST = '127.0.0.1';
    const message = Buffer.from(data);

    var client = dgram.createSocket('udp4');
    client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
        if (err)
            throw err;
    })

    client.on('message', function (message, remote) {
        var reply = message.toString('utf8')
        client.close();
    });
}



const bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// TODO keep track of all the songs added to be displayed and deleted

// function to send udp message to C with the new file name to be added to the list
// TODO

// Upload Endpoint - requests are sent to this endpoint from React
app.post('/upload', (req, res) => {
    if(req.files == null) {
        return res.status(400).json({msg: 'No file uploaded - You need to select a file'})
    }

    const file = req.files.file;

    const singer_name = req.body.singer;
    const song_name = req.body.album;
    const album_name = req.body.song;

    const file_pat = `/songs/${file.name}`;

    const c_UDP_values = `${'newSong'}\n${file_pat}\n${song_name}\n${singer_name}\n${album_name}`;


    if (fs.existsSync(`${__dirname}/../songs/${file.name}`)) {
        res.status(400).json({msg: 'No file uploaded - The file is already uploaded!'})
    }
    else if (path.extname(file.name) != '.mp3') {
        return res.status(400).json({msg: 'No file uploaded - The file should be mp3'});
    }
    else {
        file.mv(`${__dirname}/../songs/${file.name}`, err => {
            if(err) {
                return res.status(500).send(err);
            }
            // convert the file into wav.
            ffmpeg(`${__dirname}/../songs/${file.name}`).toFormat('wav').save(`${__dirname}/../songs/${file.name.slice(0, -4)}.wav`);
            // Call the UDP message handler here TODO
            

            sendUDP(c_UDP_values);

            setTimeout(() => {
                return res.status(200).json({ result: true, msg: "File uploaded",fileName: file.name, filePath: `/uploads/${file.name}`});
            }, 
                100);
        });
    }
});

app.post('/delete', (req, res) => {
    console.log('file deleted');
    const song_name = req.body.name;
    const filePathMP3 = `${__dirname}/../songs/${song_name}`;
    
    fs.unlink(filePathMP3, (err) => {
        if (err) {
            console.error(err);
        }
    });
    
    const filePathWav = `${__dirname}/../songs/${song_name.slice(0, -4)}.wav`;
    fs.unlink(filePathWav, (err) => {
    if (err) {
        console.error(err);
    }
    });
    // Send a UDP message to the C program to delete the song

    

    return res.status(200).json({result : true, mdg: 'Song deleted!'})
});

app.listen(5000, () => console.log('Server started'));