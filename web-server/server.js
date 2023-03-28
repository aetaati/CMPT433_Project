const express = require('express');
const fileUpload = require('express-fileupload');

const app = express();

app.use(fileUpload());

// Upload Endpoint - requests are sent to this endpoint from React
app.post('/upload', (req, res) => {
    if(req.files == null) {
        return res.status(400).jason({msg: 'No file uploaded'})
    }
    const file = req.files.file;
    file.mv(`${__dirname}/client/public/uploads/${file.name}`, err => {
        if(err) {
            console.error(err);
            return res.status(500).send(err);
        }
        // Error here
        res.jason({ fileName: file.name, filePath: `/uploads/${file.name}`}); 
    });

});

app.listen(5000, () => console.log('Server started'));