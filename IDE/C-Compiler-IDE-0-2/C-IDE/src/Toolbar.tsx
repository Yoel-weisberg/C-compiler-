import "./styles/Toolbar.css";
import React, { useState } from 'react';
import {  openFile, useFilesContext } from "./FileManager";
import {findFileToCompile} from "./Compile";
import { useTerminal } from "./TerminalProvider";
// import { useTerminal } from "./TerminalManager";
//import { useSizeContext } from "./DisplayManager";

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
        </div>
    );
};