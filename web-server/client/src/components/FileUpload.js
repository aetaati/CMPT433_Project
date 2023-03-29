import React, { Fragment, useState } from "react";
import axios from "axios";
import Message from "./Message";

const FileUpload = () => {
  // Hooks
  const [file, setFile] = useState("");
  const [fileName, setFileName] = useState("Choose File");
  const [uploadedFile, setUploadedFile] = useState({}); // TODO: add another hook to keep the array of all files stored - maybe in server??
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
    setFile(e.target.files[0]);
    setFileName(e.target.files[0].name);
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
      formData.append("file", file);
      // TODO: maybe add other information here to the formData - i.e. singer, album, etc.

      try {
        const res = await axios.post("/upload", formData, {
          headers: {
            "Content-Type": "multipart/form-data",
          },
        });
        const { fileName, filePath } = res.data;
        setUploadedFile({ fileName, filePath });

        setMessage("File Uploaded!");
      } catch (err) {
        if (err.response.status === 500) {
          setMessage("There was a problem with the server");
        } else {
          setMessage(err.response.data.msg);
        }
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
              <label for="singerName">Singer:</label>
              <input
                type="text"
                className="custom-name-input ml-1 display:table-cell"
                id="singerName"
                placeholder={singerName}
                onChange={onSingerNameChange}
              />
              <br />
              <label for="singerName">Album:</label>
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
              {fileName}
            </label>
          </div>

          <input
            type="submit"
            value="Upload"
            className="btn btn-primary btn-block mt-4"
          />
        </form>
      }

      {uploadedFile ? (
        <div className="row mt-5">
          <div className="col-md-6 m-auto">
            <h3 className="text-center">Uploaded songs</h3>
            {/* for each file uploaded ?? */}
            <h2 className="text-center">{uploadedFile.fileName}</h2>
          </div>
        </div>
      ) : null}
    </Fragment>
  );
};

export default FileUpload;