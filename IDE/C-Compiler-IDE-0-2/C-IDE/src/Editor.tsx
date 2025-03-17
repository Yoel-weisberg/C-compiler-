import React, { useEffect, useState } from "react";
import Editor from "@monaco-editor/react";
import { useFileOperations } from "./FileEventsHandler";
import { useLiveShare } from "./FileContentContext";
import HostLiveShare from "./HostLiveShare";

export const CodeEditor: React.FC = () => {
  const { isHost, fileData: liveShareFileData, setFileData: setLiveShareFileData } = useLiveShare();
  const { fileData: localFileData, setFileData: setLocalFileData } = useFileOperations();
  
  // Use liveShareFileData when connected, otherwise use localFileData
  const [editorContent, setEditorContent] = useState<string>(localFileData || "");
  
  // Update editor content when either file data source changes
  useEffect(() => {
    if (liveShareFileData) {
      // If we're receiving data through live share, prioritize it
      setEditorContent(liveShareFileData);
      // Also update local file data to keep them in sync
      setLocalFileData(liveShareFileData);
    } else if (localFileData) {
      // Otherwise use local file data
      setEditorContent(localFileData);
    }
  }, [liveShareFileData, localFileData, setLocalFileData]);
  
  const handleEditorChange = (value: string | undefined) => {
    const newContent = value || "";
    setEditorContent(newContent);
    
    // Update both local and live share state
    setLocalFileData(newContent);
    
    // If we're hosting, also update the live share data to broadcast changes
    if (isHost) {
      setLiveShareFileData(newContent);
    }
    
    console.log("Editor content changed:", newContent.substring(0, 50) + (newContent.length > 50 ? "..." : ""));
  };
  
  return (
    <div className="editor-container">
      {isHost && <HostLiveShare />}
      
      {/* Connection status indicator */}
      {liveShareFileData && !isHost && (
        <div className="live-share-status">
          Connected to live share session
        </div>
      )}
      
      <Editor
        className="monaco-editor"
        height="100vh"
        defaultLanguage="c"
        value={editorContent}
        onChange={handleEditorChange}
        theme="vs-dark"
        options={{
          fontSize: 14,
          minimap: { enabled: true },
          wordWrap: "on",
          scrollBeyondLastLine: false,
          readOnly: !isHost && !!liveShareFileData, // Make read-only when connected as client
        }}
      />
    </div>
  );
};