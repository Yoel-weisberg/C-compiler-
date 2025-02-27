import "./styles/Toolbar.css";
import React, { useState } from 'react';
import {  openFile, useFilesContext } from "./FileManager";

export const Toolbar: React.FC = () => {

    const [visibleDropdown, setVisibleDropdown] = useState<string | null>(null);

    const showDropdown = (dropdownName: string) => {
        setVisibleDropdown(dropdownName);
    };

    const hideDropdown = () => {
        setVisibleDropdown(null);
    };

    const {addFile} = useFilesContext();
    const handleOpenFile = async () => {
        console.log("Handling OpenFile!!!!!!!!!!!!!!!")
        const result = await openFile(addFile);  // Call openFile and wait for the result
        if (result) {
            // Optionally, add any additional logic after the file is opened
            console.log("File opened successfully:", result);
        }
    };

    const handleSaveFile = () => {
        // Implement save functionality here
    };

    const handleCompile = () => {
        // Implement compile functionality here
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