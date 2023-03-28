import './App.css';
import FileUpload from './components/FileUpload';
import React from 'react';

const App = () => 
    <div className="container mt-4">
      <h4 className="display-4 text-center mb-4">
        <i className="fab fa-react"></i> BeaglePod Song Upload
      </h4>

    <FileUpload></FileUpload>
    </div>;

export default App;
