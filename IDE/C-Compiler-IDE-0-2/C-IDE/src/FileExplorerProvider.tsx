import { createContext, useState, useContext, ReactNode } from "react";
import { invoke } from "@tauri-apps/api/core";
import { open} from '@tauri-apps/plugin-dialog';
import React from "react";
import { openFile, useFilesContext } from "./FileManager";

interface FileNode {
    name: string;
    path: string;
    is_directory: boolean;
    children?: FileNode[];
}
  
interface FileExplorerContextType {
    fileTree: FileNode | null;
    openFolder: () => void;
} 

const FileExplorerContext = createContext<FileExplorerContextType | undefined>(undefined);  

export const FileExplorerProvider: React.FC<{ children: ReactNode }> = ({ children }) => {
    const [fileTree, setFileTree] = useState<FileNode | null>(null); 
     
    const loadDirectory = async (path: string) => {
        try {
            const data = await invoke<FileNode[]>("read_directory", { path });
            setFileTree({
                name: path.split("/").pop() || "root",
                path,
                is_directory: true,
                children: data,
            });
            console.log(data);
        } catch (error) {
        console.error("Failed to load files:", error);
        }
    };  

    const openFolder = async () => {
        const dirPath = await open({
            multiple: false,
            directory: true,
        });  
        if (!dirPath) {
            return;
        }  
        loadDirectory(dirPath as string); // Load selected folder's contents
    };  
    return (
        <FileExplorerContext.Provider value={{ fileTree, openFolder }}>
            {children}
            {fileTree && <RenderTree node={fileTree} level={0} />}
        </FileExplorerContext.Provider>
    );
};  

export const useFileExplorer = (): FileExplorerContextType => {
  const context = useContext(FileExplorerContext);
  if (!context) {
    throw new Error("useFileExplorer must be used within a FileExplorerProvider");
  }
  return context;
};


export const RenderTree: React.FC<{ node: FileNode; level: number }> = ({ node, level }) => {
    const [isOpen, setIsOpen] = useState(false); // State to control folder visibility

    const toggleFolder = () => {
        setIsOpen(prev => !prev); // Toggle the visibility
    };

    const {addFile} = useFilesContext();
    const handleOpenCurrentFile = async () => {
        console.log("Handling: " + node.path);
        const result = await openFile(addFile, node.path);  // Call openFile and wait for the result
        if (result) {
            console.log("File opened successfully:", result);
        }
    }

    const displayName = node.is_directory ? `${node.name.split("\\").pop()}` : ` - ${node.name}`; 

    return (
        <div className="file-item-container">
            <div 
                className="file-item"
                style={{ paddingLeft: `${level * 15}px` }} // Indent dynamically
            >
                {node.is_directory && (
                    <button onClick={toggleFolder} className="folder-toggle-btn">
                        {isOpen ? '−' : '+'}{` ${displayName}`}
                    </button>
                )}
                {!node.is_directory && (
                <button className="file-item-button" onClick={handleOpenCurrentFile}> 
                    {displayName}
                </button>
                )}
            </div>
            
            {/* Render only if folder is open */}
            {node.is_directory && isOpen && node.children && (
                <div className="file-children">
                    {node.children.map((child) => (
                        <RenderTree key={child.path} node={child} level={level + 1} />
                    ))}
                </div>
            )}
        </div>
    );
};