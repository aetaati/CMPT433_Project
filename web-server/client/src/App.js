import './App.scss';
import FileUpload from './components/FileUpload';
import React, { useState } from 'react';
import FileList from './components/FileList';

function App() {
    const [files, setFiles] = useState([])

    const removeFile = (filename) => {
      setFiles(files.filter(file => file.name !== filename))
    }

    return (
      <div className="container mt-4">
        <h4 className="display-4 text-center mb-4">
          <i className="fab fa-react"></i> BeaglePod Song Upload
        </h4>

        <FileUpload files={files} setFiles={setFiles}
          removeFile={removeFile} />
        <FileList files={files} removeFile={removeFile} />

      </div>
    );
}

export default App;
