import React, { Fragment, useState } from "react";
import axios from "axios";
import Message from "./Message";

const FileUpload = ({files, setFiles, removeFile}) => {
  // Hooks
  const [fileLocal, setFileLocal] = useState("");
  const [fileNameLocal, setFileNameLocal] = useState("Choose File");
  const [message, setMessage] = useState("");
  const [singerName, setSingerName] = useState("Enter singer name here...");
  const [albumName, setAlbumName] = useState("Enter album name here...");

  const onSingerNameChange = (e) => {
    setSingerName(e.target.value);
  };

  const onAlbumNameChange = (e) => {
    setAlbumName(e.target.value);
  };

  const onFileChange = (e) => {
    const file = e.target.files[0];
    if(!file) return;
    file.isUploading = true;
    setFileLocal(file);
    setFileNameLocal(file.name);

    // setFiles([...files], fileLocal)
  };

  const onFileSubmit = async (e) => {
    e.preventDefault();
    if (
      albumName === "Enter album name here..." ||
      singerName === "Enter singer name here..." ||
      albumName === "" ||
      singerName === ""
    ) {
      setMessage("No file uploaded - All fields should be provided");
    } else {
      
      
      const formData = new FormData();
      formData.append('file', fileLocal, fileLocal.name);
      // TODO: maybe add other information here to the formData - i.e. singer, album, etc.

      try {
        await axios.post("/upload", formData, {
          headers: {
            "Content-Type": "multipart/form-data",
          },
        });
        fileLocal.isUploading = false;
        console.log(fileLocal)
        setFiles([...files, fileLocal])
        setMessage("File Uploaded!");
        console.log(files)
      } catch (err) {
        if (err.response.status === 500) {
          setMessage("There was a problem with the server");
        } else {
          setMessage(err.response.data.msg);
        }
        removeFile(fileLocal.name);
      }
    }
  };

  return (
    <Fragment>
      {message ? <Message msg={message} /> : null}
      {
        <form onSubmit={onFileSubmit}>
          {
            <div>
              <label htmlFor="singerName">Singer:</label>
              <input
                type="text"
                className="custom-name-input ml-1 display:table-cell"
                id="singerName"
                placeholder={singerName}
                onChange={onSingerNameChange}
              />
              <br />
              <label htmlFor="singerName">Album:</label>
              <input
                type="text"
                className="custom-name-input ml-1"
                id="singerName"
                placeholder={albumName}
                onChange={onAlbumNameChange}
              />
              <br />
            </div>
          }
          <div className="custom-file mb4">
            <input
              type="file"
              className="custom-file-input"
              id="customFile"
              onChange={onFileChange}
            />
            <label className="custom-file-label" htmlFor="customFile">
              {fileNameLocal}
            </label>
          </div>

          <input
            type="submit"
            value="Upload"
            className="btn btn-primary btn-block mt-4"
          />
        </form>
      }
      
    </Fragment>
  );
};

export default FileUpload;