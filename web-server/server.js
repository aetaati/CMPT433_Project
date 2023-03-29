const express = require('express');
const fileUpload = require('express-fileupload');
const fs = require('fs');
var path = require('path')
const app = express();

const ffmpegPath = require('@ffmpeg-installer/ffmpeg').path;
const ffmpeg = require('fluent-ffmpeg');
ffmpeg.setFfmpegPath(ffmpegPath);

const uploadDirectory = getUserHome()

function getUserHome() {
    return process.env[(process.platform == 'win32') ? 'USERPROFILE' : 'HOME'];
  }

app.use(fileUpload());

// TODO keep track of all the songs added to be displayed and deleted

// function to send udp message to C with the new file name to be added to the list
// TODO

// Upload Endpoint - requests are sent to this endpoint from React
app.post('/upload', (req, res) => {
    if(req.files == null) {
        return res.status(400).json({msg: 'No file uploaded - You need to select a file'})
    }
    const file = req.files.file;
    if (fs.existsSync(`${__dirname}/client/public/uploads/${file.name}`)) {
        res.status(400).json({msg: 'No file uploaded - The file is already uploaded!'})
    }
    else if (path.extname(file.name) != '.mp3') {
        return res.status(400).json({msg: 'No file uploaded - The file should be mp3'});
    }
    else {
        file.mv(`${__dirname}/client/public/uploads/${file.name}`, err => {
            if(err) {
                return res.status(500).send(err);
            }
            res.json({ fileName: file.name, filePath: `/uploads/${file.name}`}); 
            // convert the file into wav.
            ffmpeg(`${__dirname}/client/public/uploads/${file.name}`).toFormat('wav').save(`${__dirname}/../songs/${file.name.slice(0, -4)}.wav`);
            // Call the UDP message handler here TODO
        });
    }
});

app.listen(5000, () => console.log('Server started'));