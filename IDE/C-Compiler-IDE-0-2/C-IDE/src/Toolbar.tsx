import "./styles/Toolbar.css";
import React, { useState } from "react";
// import { useFileContext } from "./FileEventsHandler";
import { useFileOperations } from "./FileEventsHandler";
import { openFile, saveFile } from "./FileEventsHandler";
import { handleCompile } from "./Compile";
import JoinLiveShare from "./JoinLiveShare";
import { useLiveShare } from "./FileContentContext";
export const Toolbar: React.FC = () => {
  // const { setFileList } = useFileContext();
  const { fileData, setFileData } = useFileOperations();
  const [visibleDropdown, setVisibleDropdown] = useState<string | null>(null);
  const {setIsHost, isHost} = useLiveShare();
  const showDropdown = (dropdownName: string) => {
    setVisibleDropdown(dropdownName);
  };


  const hideDropdown = () => {
    setVisibleDropdown(null);
  };

  const handleOpenFile = async () => {
    const file = await openFile();
    if (file && file.content) {
      setFileData(file.content);
    }
  };

  const handleSaveFile = async () => {
    await saveFile(fileData);
  };

  const handleJoinLiveShare = () => {
    let peerId : string = prompt("what is the peer id you want to connect to? ") || "";
    JoinLiveShare({peerId});
  }

  return (
    <div className="toolbar">
      {/* File Dropdown */}
      <div
        className="dropdown"
        onMouseEnter={() => showDropdown("file")}
        onMouseLeave={hideDropdown}
      >
        <button className="toolbar-button">File</button>
        {visibleDropdown === "file" && (
          <ul className="dropdown-list">
            <li>
              <button onClick={handleOpenFile}>Open File</button>
            </li>
            <li>
              <button onClick={handleSaveFile}>Save File</button>
            </li>
            <li>
              <button>New File</button>
            </li>
          </ul>
        )}
      </div>

      {/* Other Buttons */}
      <button className="toolbar-button">Does Nothing</button>
      <button className="toolbar-button">Does Nothing</button>

      {/* live share button */}
      <div
        className="dropdown"
        onMouseEnter={() => showDropdown("liveShare")}
        onMouseLeave={hideDropdown}
      >
       <button className="toolbar-button">Live share</button>
        {visibleDropdown === "liveShare" && (
          <ul className="dropdown-list">
            <li>
              <button onClick={() => {setIsHost(!isHost)}}>{isHost ? "not host" : "host live share"}</button>
            </li>
            <li>
              <button onClick={handleJoinLiveShare}>Join Live Share</button>
            </li>
          </ul>
        )}
      </div>
      {/* Run Dropdown */}
      <div
        className="dropdown"
        onMouseEnter={() => showDropdown("run")}
        onMouseLeave={hideDropdown}
      >
        <button className="toolbar-button">Run</button>
        {visibleDropdown === "run" && (
          <ul className="dropdown-list">
            <li>
              <button onClick={handleCompile}>Compile</button>
            </li>
            <li>
              <button>Run Executable</button>
            </li>
          </ul>
        )}
      </div>
    </div>
  );
};
