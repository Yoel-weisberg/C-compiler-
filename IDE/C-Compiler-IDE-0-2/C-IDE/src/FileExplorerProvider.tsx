import React, { createContext, useState, useContext, ReactNode } from "react";
import { invoke } from "@tauri-apps/api/core";
import { open} from '@tauri-apps/plugin-dialog';

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