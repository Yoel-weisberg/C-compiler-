import "./styles/FileExplorer.css";
import FolderTree from "react-folder-tree";
import { RenderTree, useFileExplorer } from "./FileExplorerProvider";  // Adjust import to your file structure

const FileExplorer: React.FC = () => {
  const { fileTree } = useFileExplorer();

  return (
      <div className="file-explorer">
          <div className="file-explorer-header">
              <span>Explorer</span>
          </div>
          <div className="file-explorer-content">
              {fileTree ? (
                  <RenderTree node={fileTree} level={0} />  
              ) : (
                  <p className="empty-explorer">No Folder Opened</p>
              )}
          </div>
      </div>
  );
};

export default FileExplorer;