import "./styles/FileExplorer.css";
import FolderTree from "react-folder-tree";
import { useFileExplorer } from "./FileExplorerProvider";  // Adjust import to your file structure

const FileExplorer: React.FC = () => {
  const { fileTree } = useFileExplorer();

  return (
    <div className="file-explorer">
      <div className="file-explorer-header">
        <span>Explorer</span>
        {/* <button onClick={openFolder}>Open Folder</button> */}
      </div>
      <div className="file-explorer-content">
        {fileTree ? (
          <FolderTree data={fileTree} onChange={() => {}} className="file-item" />
        ) : (
          <p>Loading files...</p>
        )}
      </div>
    </div>
  );
};

export default FileExplorer;