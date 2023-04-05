import './App.scss';
import FileUpload from './components/FileUpload';
import React, { useState } from 'react';
import FileList from './components/FileList';
import Message from "./components/Message";

function App() {
    const [files, setFiles] = useState([]);
    const [message, setMessage] = useState("");

    const removeFile = (filename) => {
      setFiles(files.filter(file => file.name !== filename))
    }

    return (
      <div className="container mt-4">
        <h4 className="display-4 text-center mb-4">
          <i className="fab fa-react"></i> BeaglePod Song Upload
        </h4>

        {message ? <Message msg={message} /> : null}
        <FileUpload files={files} setFiles={setFiles} setMessage={setMessage} />
        <FileList files={files} removeFile={removeFile} />

      </div>
    );
}

export default App;
