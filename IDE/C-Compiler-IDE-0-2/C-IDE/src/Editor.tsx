import React, { useEffect, useState } from "react";
import Editor from "@monaco-editor/react";
import { useFileContext, useFileOperations } from "./FileEventsHandler";

export const CodeEditor: React.FC = () => {
    const { fileList } = useFileContext();
    const { fileContent, setFileContent } = useFileOperations();
    const [editorContent, setEditorContent] = useState<string>(fileContent || "");
  
    // Update editor content when a new file is loaded
    useEffect(() => {
      if (fileContent) {
        setEditorContent(fileContent);
      }
    }, [fileContent]);
  
    const handleEditorChange = (value: string | undefined) => {
      setEditorContent(value || "");
      setFileContent(value || ""); // Update global state
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
  
