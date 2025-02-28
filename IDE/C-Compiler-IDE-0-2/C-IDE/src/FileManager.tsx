import { createContext, useState, useContext } from "react";
import { open} from '@tauri-apps/plugin-dialog';
import { readTextFile, writeTextFile } from '@tauri-apps/plugin-fs';


// Tracks state of file 
interface FileState {
  location: string;
  lastSavedVersion: File;
  currentVersion: File;
}

// All opened files in the current session
export interface OpenFiles {
  files: FileState[]; // All opened files
  currentFile: number; // Index of currently viewed file
}


interface FilesContextType {
  openFiles: OpenFiles;
  setOpenFiles: (update: (prev: OpenFiles | null) => OpenFiles) => void;
  addFile: (newFile: FileState) => void;
  updateFile: (index: number, updateFile: Partial<FileState>) => void;
  setCurrentFile: (index: number) => void; 
  saveFile: () => Promise<void>;
}

export const FilesContext = createContext<FilesContextType | null>(null);

export const useFilesContext = (): FilesContextType => {
  const context = useContext(FilesContext);
  if (!context) {
    throw new Error('useFilesContext must be used within a FilesProvider');
  }
  return context;
};


// Provider component
export const FilesProvider = ({children}: {children: React.ReactNode}) => {
  const [openFiles, setOpenFilesState] = useState<OpenFiles>({
    files: [],
    currentFile: -1,
  });

    // Method to add a new file to the list
    const addFile = (newFile: FileState) => {
      setOpenFilesState(prev => ({
        files: [...prev.files, newFile],
        currentFile: prev.files.length, // Automatically set the new file as current
      }));
    };
  
    // Method to update a file at a given index
    const updateFile = (index: number, updatedFile: Partial<FileState>) => {
      setOpenFilesState(prev => {
        const updatedFiles = [...prev.files];
        updatedFiles[index] = { ...updatedFiles[index], ...updatedFile }; // Merge updated fields
        return { ...prev, files: updatedFiles };
      });
    };
  
    // Method to set the current file by index
    const setCurrentFile = (index: number) => {
      setOpenFilesState(prev => ({
        ...prev,
        currentFile: index,
      }));
    };

    const saveFile = async () => {
      const { files, currentFile } = openFiles;
  
      if (currentFile === -1) {
        console.error('No file is currently open.');
        return;
      }
      console.log("IN SAVE FILE");
      const fileToSave = files[currentFile];
  
      try {
        await writeTextFile(fileToSave.location, await fileToSave.currentVersion.text());
  
        updateFile(currentFile, {
          lastSavedVersion: fileToSave.currentVersion,
        });
  
        console.log(`File saved successfully to ${fileToSave.location}`);
      } catch (error) {
        console.error('Failed to save the file:', error);
      }
    };

    return (
      <FilesContext.Provider
      value={{ openFiles, setOpenFiles: setOpenFilesState, addFile, updateFile, setCurrentFile, saveFile }}
    >
      {children}
    </FilesContext.Provider>
    );
};



// File Managing method

// Open

export async function openFile(addFile: (file: FileState) => void) {
  // Open a file using the dialog
  const filePath = await open({
    multiple: false,
    directory: false,
  });

  if (filePath) {
    const content = await readTextFile(filePath as string); // Read file content
    const fileName = filePath.split('/').pop() || 'Untitled'; // Extract file name

    const newFile: FileState = {
      location: filePath as string,
      lastSavedVersion: new File([content], fileName),
      currentVersion: new File([content], fileName),
    };

    // Add the file using the function received as an argument
    addFile(newFile);

    return { content, filePath };
  }

  return null;
}

// Save

// New File