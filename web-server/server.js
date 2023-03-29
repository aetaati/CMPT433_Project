const express = require('express');
const fileUpload = require('express-fileupload');
const Mp32Wav = require('mp3-to-wav')
const fs = require('fs');
var path = require('path')




const app = express();

app.use(fileUpload());

// function to send udp message to C with the new file name to be added to the list
// TODO

// Upload Endpoint - requests are sent to this endpoint from React
app.post('/upload', (req, res) => {
    if(req.files == null) {
        return res.status(400).json({msg: 'No file uploaded - You need to select a file'})
    }
    const file = req.files.file;
    if (fs.existsSync(`${__dirname}/client/public/uploads/${file.name}`)) {
        console.log('file exists');
        res.status(400).json({msg: 'The file is already uploaded!'})
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
            console.log(`${__dirname}/client/public/uploads/${file.name}`)
            const mp32Wav = new Mp32Wav(`${__dirname}/client/public/uploads/${file.name}`)
        });
    }
});

app.listen(5000, () => console.log('Server started'));