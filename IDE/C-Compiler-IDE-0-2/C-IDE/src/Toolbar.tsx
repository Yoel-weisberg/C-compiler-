import "./styles/Toolbar.css";
import React, { useState } from 'react';
// import { useFileContext } from "./FileEventsHandler";
import { useFileOperations } from './FileEventsHandler'; 
import { openFile, saveFile } from './FileEventsHandler';
import { handleCompile} from "./Compile";

export const Toolbar: React.FC = () => {
    // const { setFileList } = useFileContext();
    const { fileContent, setFileContent } = useFileOperations();

    const [visibleDropdown, setVisibleDropdown] = useState<string | null>(null);

    const showDropdown = (dropdownName: string) => {
        setVisibleDropdown(dropdownName);
    };

    const hideDropdown = () => {
        setVisibleDropdown(null);
    };

    const handleOpenFile = async () => {
        const file = await openFile();
        if (file && file.content) {
            setFileContent(file.content);
        }
    };

    const handleSaveFile = async () => {
        await saveFile(fileContent);
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

