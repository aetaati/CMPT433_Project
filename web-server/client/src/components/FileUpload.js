import React, {Fragment, useState} from 'react'
import axios from 'axios'
import Message from './Message';


const FileUpload = () => {
  const [file, setFile] = useState('');
  const [fileName, setFileName] = useState('Choose File');
  const [uploadedFile, setUploadedFile] = useState({});
  const [message, setMessage] = useState('');
  
  const onFileChange = e => {
    setFile(e.target.files[0]);
    setFileName(e.target.files[0].name);
  }

  const onFileSubmit = async e => {
    e.preventDefault();
    const formData = new FormData();
    formData.append('file', file);

    try {
        const res = await axios.post('/upload', formData, {
            headers: {
                'Content-Type': 'multipart/form-data'
            }
        });
        const {fileName, filePath} = res.data;
        setUploadedFile({fileName, filePath});

        setMessage("File Uploaded!");
    }
    catch(err) {
        if (err.response.status === 500) {
          setMessage("There was a problem with the server");
        }
        else {
          setMessage(err.response.data.msg);
        }
    }
  }

  return (
    <Fragment>
      {message ? <Message msg={message}/> : null}  
      <form onSubmit={onFileSubmit}>
        <div className="custom-file mb4">
            <input type="file" className="custom-file-input" id="customFile" onChange={onFileChange}/>
            <label className="custom-file-label" htmlFor="customFile">
                {fileName}
            </label>
        </div>

        <input type="submit" value="Upload" className='btn btn-primary btn-block mt-4'/>
      </form>
      { uploadedFile ? 
      <div className="row mt-5">
        <div className="col-md-6 m-auto">
          <h3 className="text-center">Uploaded songs</h3>
          {/* for each file uploaded ?? */}
          <h2 className="text-center">{uploadedFile.fileName}</h2>  
        </div>
      </div> : null
      }
    </Fragment>
  )
}

export default FileUpload 