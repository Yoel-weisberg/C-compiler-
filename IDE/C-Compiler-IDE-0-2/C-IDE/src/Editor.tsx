import React, { useContext, useEffect, useState } from "react";
import { FilesContext } from "./FileManager";
import Editor from "@monaco-editor/react";

export const CodeEditor: React.FC = () => {

  const { openFiles, updateFile } = useContext(FilesContext)!; // Make sure the context is not null

  // Get the current file's content (currentVersion is a File object)
  const currentFileIndex = openFiles.currentFile;
  const currentFile = currentFileIndex !== -1 ? openFiles.files[currentFileIndex] : null;

  // State for editor content (to hold text from the File object)
  const [editorContent, setEditorContent] = useState<string>("");

  // Function to read the content of a File object
  const readFileContent = (file: File) => {
    const reader = new FileReader();
    return new Promise<string>((resolve, reject) => {
      reader.onload = () => resolve(reader.result as string);
      reader.onerror = (err) => reject(err);
      reader.readAsText(file); // Read the file as text
    });
  };

  // Update editor content whenever the current file changes
  useEffect(() => {
    if (currentFile) {
      readFileContent(currentFile.currentVersion).then((content) => {
        setEditorContent(content); // Set editor content to the file content
      });
    }
  }, [currentFileIndex, currentFile]);

  // Handle content changes in Monaco editor
  const handleEditorChange = (value: string | undefined) => {
    if (value && currentFile) {
      // Convert the string back to a File object and update it
      const updatedFile = new File([value], currentFile.location, { type: "text/plain" });
      updateFile(currentFileIndex, { currentVersion: updatedFile }); // Update the file content
    }
  };

    return (
        <div className="editor-container">
            <Editor
            className="monaco-editor"
            height="100vh" // Editor height
            defaultLanguage="c" // Default language
            value={editorContent} // Editor value
            onChange={handleEditorChange} // Handle content changes
            theme="vs-dark" // Theme (vs-light, hc-black, etc.)
            options={{
                fontSize: 14,
                minimap: { enabled: false }, // Disable minimap
                wordWrap: "on",
                scrollBeyondLastLine: false,
            }}
            />
      </div>
    );
  };
  

