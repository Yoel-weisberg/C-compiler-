import React, { useEffect, useState } from "react";
import Editor from "@monaco-editor/react";
import { useFileOperations } from "./FileEventsHandler";

export const CodeEditor: React.FC = () => {
    // const { fileList } = useFileContext();

    const { fileData, setFileData } = useFileOperations();
    const [editorContent, setEditorContent] = useState<string>(fileData || "");
  
    // Update editor content when a new file is loaded
    useEffect(() => {
      if (fileData) {
        setEditorContent(fileData);
      }
    }, [fileData]);
  
    const handleEditorChange = (value: string | undefined) => {
      setEditorContent(value || "");
      setFileData(value || ""); // Update global state
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
  
