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
            setTimeout(() => {
                return res.status(200).json({ result: true, msg: "File uploaded",fileName: file.name, filePath: `/uploads/${file.name}`});
            }, 
                3000);
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
        else {
            console.log(`${filePathMP3} has been deleted`);
        }
    });
    
    const filePathWav = `${__dirname}/../songs/${song_name.slice(0, -4)}.wav`;
    fs.unlink(filePathWav, (err) => {
    if (err) {
        console.error(err);
    }
    else {
        console.log(`${filePathWav} has been deleted`);
    }
    });
    // Send a UDP message to the C program to delete the song
    return res.status(200).json({result : true, mdg: 'Song deleted!'})
})

app.listen(5000, () => console.log('Server started'));