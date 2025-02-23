import React, { createContext, useContext, useState, ReactNode } from 'react';
import { save } from '@tauri-apps/plugin-dialog';
import { open } from '@tauri-apps/plugin-dialog';
import { readTextFile } from '@tauri-apps/plugin-fs';
import { writeTextFile } from '@tauri-apps/plugin-fs';

export async function openFile() {
  // Open a file using the dialog
  const filePath = await open({
    multiple: false,  // Allow only one file to be selected
    directory: false, // Don't allow directories
  });

  // Check if the user selected a file
  if (filePath) {
    const content = await readTextFile(filePath as string); // Read the file's content
    return { content, filePath };
  }

  // Return an empty object if no file was selected
  return null;
}


export const saveFile = async (fileContent: string) => {
  try {
    // Open the save dialog to choose where to save the file
    const filePath = await save({
      filters: [
        {
          name: 'C Source Files',
          extensions: ['c', 'h'],
        },
      ],
    });

    if (filePath) {
      // Save the content to the selected file
      await writeTextFile(filePath, fileContent); // Write content to file
      console.log('File saved successfully');
    }
  } catch (error) {
    console.error('Error saving file:', error);
  }
};


interface FileState {
    fileList: FileList | null;
    setFileList: React.Dispatch<React.SetStateAction<FileList | null>>;
}

interface FileOperationsContextType {
    saveFile: () => void;
    setFileContent: (content: string) => void;
    fileContent: string;
}

const FileOperationsContext = createContext<FileOperationsContextType | undefined>(undefined);
const FileContext = createContext<FileState | undefined>(undefined);

export const useFileOperations = (): FileOperationsContextType => {
    const context = useContext(FileOperationsContext);
    if (!context) {
      throw new Error("useFileOperations must be used within a FileOperationsProvider");
    }
    return context;
  };

// Create a custom hook to access the context
export const useFileContext = () => {
    const context = useContext(FileContext);
    if (!context) {
      throw new Error("useFileContext must be used within a FileProvider");
    }
    return context;
};

// Create a provider component to wrap around your app
interface FileProviderProps {
    children: ReactNode;
}  


export const FileProvider: React.FC<FileProviderProps> = ({ children }) => {
    const [fileList, setFileList] = useState<FileList | null>(null);
    const [fileContent, setFileContent] = useState<string>("");

    // Save file functionality
    const saveFile = () => {
        if (!fileContent) return;
        const file = new Blob([fileContent], { type: 'text/plain' });
        const fileURL = URL.createObjectURL(file);
        const a = document.createElement('a');
        a.href = fileURL;
        a.download = "editedFile.c"; // You can use the original file name if needed
        a.click();
        URL.revokeObjectURL(fileURL); // Clean up
    };

    return (
        <FileContext.Provider value={{ fileList, setFileList }}>
            <FileOperationsContext.Provider value={{ saveFile, setFileContent, fileContent }}>
                {children}
            </FileOperationsContext.Provider>
        </FileContext.Provider>
    );
};



