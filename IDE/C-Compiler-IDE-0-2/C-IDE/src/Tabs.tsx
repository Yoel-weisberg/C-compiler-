import "./styles/Tabs.css";
import React, { useContext } from "react";
import { FilesContext } from "./FileManager"; // Assuming you have this context

export const Tabs: React.FC = () => {
  const { openFiles, setCurrentFile } = useContext(FilesContext)!;

  return (
    <div className="open-tabs">
      {openFiles.files.map((file, index) => (
        <div
          key={index}
          className={`tab ${openFiles.currentFile === index ? "active" : ""}`}
          onClick={() => setCurrentFile(index)} // Set the current file when tab is clicked
        >
          {file.lastSavedVersion.name}
        </div>
      ))}
    </div>
  );
};
