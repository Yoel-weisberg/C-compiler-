import "./styles/Toolbar.css";
import React, { useEffect, useState } from 'react';
import {  openFile, useFilesContext } from "./FileManager";
import {findFileToCompile} from "./Compile";
import { useTerminal } from "./TerminalProvider";
import { useFileExplorer } from "./FileExplorerProvider";
import { useLiveShare } from "./FileContentContext";
import { Peer, DataConnection } from "peerjs";

export const Toolbar: React.FC = () => {


    // --------- Dropdown Managing ---------
    const [visibleDropdown, setVisibleDropdown] = useState<string | null>(null);

  const showDropdown = (dropdownName: string) => {
    setVisibleDropdown(dropdownName);
  };

  const hideDropdown = () => {
    setVisibleDropdown(null);
  };

    // ------------------------------------

    // ------ File Dropdown Functions -----
    const {saveFile} = useFilesContext();
    const {addFile} = useFilesContext();
    const handleOpenFile = async () => {
        const result = await openFile(addFile);  // Call openFile and wait for the result
        if (result) {
            console.log("File opened successfully:", result);
        }
    };

    const {getCurrentFile} = useFilesContext();
    //const {printToTerminal} = useTerminal();

    
    const handleCreateFile = () => {
            console.log("Does Nothing, TODO");
            // Create File in the current directory 
            // Open file for display
    };

    const {openFolder} = useFileExplorer();

    // ------------------------------------

    // ------ View Dropdown Functions -----
    const { toggleTerminal, isTerminalVisible, writeToTerminal } = useTerminal();

    // ------------------------------------
    
    // ------ Run Dropdown Functions -----

        const handleCompile = async () => {
        const str = await findFileToCompile(getCurrentFile);
        console.log(str);
        writeToTerminal(str); // Show output in terminal
    };

    // ------------------------------------
    // ------ Live Share Dropdown Functions -----
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
                            <button onClick={handleOpenFile}>
                                Open File
                            </button>
                        </li>
                        <li>
                            <button onClick={saveFile}>
                                Save File
                            </button>
                        </li>
                        <li>
                            <button onClick={handleCreateFile}>
                                New File
                            </button>
                        </li>
                        <li>
                            <button onClick={openFolder}>
                                open Folder
                            </button>
                        </li>
                    </ul>
                )}
            </div>

            {/* View Dropdown */}
            <div 
            className="dropdown" 
            onMouseEnter={() => showDropdown("view")}
            onMouseLeave={hideDropdown}
            >
                <button className="toolbar-button">View</button>
                {visibleDropdown === "view" && (
                    <ul className="dropdown-list">
                        <li>
                            <button onClick={() => {toggleTerminal();}}>
                                {isTerminalVisible ? 'Hide Terminal' : 'Show Terminal'}
                            </button>
                        </li>
                        <li>
                            <button>
                                Add something
                            </button>
                        </li>
                    </ul>
                )}
            </div>
            <button className="toolbar-button">Does Nothing</button>

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
                            <button onClick={handleCompile}>
                                Compile
                            </button>
                        </li>
                        <li>
                            <button>
                                Run Executable
                            </button>
                        </li>
                    </ul>
                )}
            </div>

            <div 
                className="dropdown" 
                onMouseEnter={() => showDropdown("liveShare")}
                onMouseLeave={hideDropdown}
            >
                <button className="toolbar-button">Live Share</button>
                {visibleDropdown === "liveShare" && (
                    <ul className="dropdown-list">
                        <li>
                            <button onClick={handleHostLiveShare}>
                                {isHost ? "Stop hosting" : "Host live share"}
                            </button>
                            {isHost && peerId && (<div className="peer-id-display">Your ID: {peerId}</div>)}
                        </li>
                        <li>
                        <button onClick={handleJoinLiveShare}>Join Live Share</button>
                            {targetPeerId && (<div className="peer-id-display">Connected to: {targetPeerId}</div>)}
                        </li>
                    </ul>
                )}
            </div>
        </div>
    );
};