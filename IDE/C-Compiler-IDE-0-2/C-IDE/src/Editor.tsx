import "./styles/Editor.css";
import React, { useContext, useEffect, useState } from "react";
import { FilesContext } from "./FileManager";
import Editor, { OnMount } from "@monaco-editor/react";
import { useTerminal } from "./TerminalProvider";
import * as monaco from 'monaco-editor';
import { useLiveShare } from "./FileContentContext";

// import { useTerminal } from "./TerminalManager";
//import { useSizeContext } from "./DisplayManager";

export const CodeEditor: React.FC = () => {
    const { openFiles, updateFile } = useContext(FilesContext)!; // Make sure the context is not null
    // Get the current file's content (currentVersion is a File object)
    const currentFileIndex = openFiles.currentFile;
    const currentFile = currentFileIndex !== -1 ? openFiles.files[currentFileIndex] : null;
    // State for editor content (to hold text from the File object)
    const [editorContent, setEditorContent] = useState<string>("");
    const { isHost, fileData: liveShareFileData, setFileData: setLiveShareFileData } = useLiveShare();

    //const {editorHeight} = useSizeContext();
    const { terminalHeight } = useTerminal(); 

    // Function to read the content of a File object
    const readFileContent = (file: File) => {
        const reader = new FileReader();
        return new Promise<string>((resolve, reject) => {
            reader.onload = () => resolve(reader.result as string);
            reader.onerror = (err) => reject(err);
            reader.readAsText(file); // Read the file as text
        });
    };

    // Apply the custom theme when Monaco loads
    const handleEditorMount: OnMount = (editor, monaco) => {
      monaco.editor.defineTheme("myCustomTheme", {
          base: "vs-dark",
          inherit: true,
          rules: [],
          colors: {
              // "editor.background": "#1E1E1E",
              // "editor.foreground": "#D4D4D4",
              // "editor.lineHighlightBackground": "#33333355",
              // "editorCursor.foreground": "#A0A0A0",
              "editorWidget.border": "#3e3e3e",
              // "editor.lineHighlightBorder": "#FF0000",
          },
      });

      // Apply the theme after defining it
      monaco.editor.setTheme("myCustomTheme");
  };


    // Update editor content whenever the current file changes
    useEffect(() => {
        if (liveShareFileData) {
            // If we're receiving data through live share, prioritize it
            setEditorContent(liveShareFileData);

        }
        if (currentFile) {
            readFileContent(currentFile.currentVersion).then((content) => {
                setEditorContent(content); // Set editor content to the file content
            });
        }
        
    }, [currentFileIndex, currentFile, liveShareFileData]);

    // Handle content changes in editor
    const handleEditorChange = (value: string | undefined) => {
        if (value && currentFile) {
            // Convert the string back to a File object and update it
            let updatedFile = null;
            if (currentFile.location.endsWith(".c") ) {
                updatedFile = new File([value], currentFile.location, { type: "text/x-csrc" });
            }
            else { // If it's a headr file (".h")
                updatedFile = new File([value], currentFile.location, { type: "text/x-chdr" });
            }
            updateFile(currentFileIndex, { currentVersion: updatedFile }); // Update the file content
        }

            const newContent = value || "";
        setEditorContent(newContent);
        
        // Update both local and live share state
        // setLocalFileData(newContent);
        
        // If we're hosting, also update the live share data to broadcast changes
        if (isHost) {
        setLiveShareFileData(newContent);
        }
        
        console.log("Editor content changed:", newContent.substring(0, 50) + (newContent.length > 50 ? "..." : ""));
    };


    return (
      <div className="editor-container">
            <Editor
            className="monaco-editor"
            //height={`calc(100vh - ${terminalHeight}px - 19px)`} // Editor height (with the terminals' border)
            height={`calc(100vh - ${terminalHeight}px)`} 
            defaultLanguage="c" // Default language
            value={editorContent} // Editor value
            onChange={handleEditorChange} // Handle content changes
            onMount={handleEditorMount} // Apply the theme on mount
            theme="myCustomTheme" // Monaco will use this once set
            options={{
                fontSize: 14,
                minimap: { enabled: false }, 
                wordWrap: "on",
                scrollBeyondLastLine: false,
            }}
            />
      </div>
  );
};
  

