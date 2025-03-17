import "./styles/Toolbar.css";
import React, { useState, useEffect } from "react";
import { useFileOperations } from "./FileEventsHandler";
import { openFile, saveFile } from "./FileEventsHandler";
import { handleCompile } from "./Compile";
import { connectToPeer } from "./JoinLiveShare"; // Import the function, not the component
import { useLiveShare } from "./FileContentContext";
import { Peer, DataConnection } from "peerjs";

export const Toolbar: React.FC = () => {
  const { fileData, setFileData } = useFileOperations();
  const [visibleDropdown, setVisibleDropdown] = useState<string | null>(null);
  const { setIsHost, isHost, initialize, peerId, setFileData: setLiveShareFileData } = useLiveShare();
  const [connection, setConnection] = useState<{ peer?: Peer, conn?: DataConnection } | null>(null);
  const [targetPeerId, setTargetPeerId] = useState<string | null>(null);

  // Effect to establish connection when targetPeerId changes
  useEffect(() => {
    if (targetPeerId) {
      // Clean up any existing connection
      if (connection) {
        if (connection.conn) connection.conn.close();
        if (connection.peer) connection.peer.destroy();
      }
      
      // Create a new peer connection
      const peer = new Peer();
      console.log(`Attempting to connect to peer: ${targetPeerId}`);
      
      peer.on('open', () => {
        const conn = peer.connect(targetPeerId);
        
        conn.on('open', function() {
          console.log('Connection established with', targetPeerId);
          
          conn.on('data', function(data: any) {
            console.log('Received data:', data);
            if (data && data.data) {
              setLiveShareFileData(String(data.data));
            }
          });
        });
        
        conn.on('error', function(err) {
          console.error(`Connection error:`, err);
        });
        
        setConnection({ peer, conn });
      });
      
      peer.on('error', function(err) {
        console.error(`Peer error:`, err);
      });
      
      // Clean up on component unmount or when targetPeerId changes
      return () => {
        if (connection) {
          if (connection.conn) connection.conn.close();
          if (connection.peer) connection.peer.destroy();
        }
      };
    }
  }, [targetPeerId, setLiveShareFileData]);

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
    const inputPeerId = prompt("What is the peer id you want to connect to?") || "";
    if (inputPeerId) {
      setTargetPeerId(inputPeerId);
    }
  };

  const handleHostLiveShare = () => {
    if (!isHost) {
      initialize(); // Initialize the peer when becoming a host
    }
    setIsHost(!isHost);
  };

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
              <button onClick={handleHostLiveShare}>
                {isHost ? "Stop hosting" : "Host live share"}
              </button>
              {isHost && peerId && (
                <div className="peer-id-display">Your ID: {peerId}</div>
              )}
            </li>
            <li>
              <button onClick={handleJoinLiveShare}>Join Live Share</button>
              {targetPeerId && (
                <div className="peer-id-display">Connected to: {targetPeerId}</div>
              )}
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