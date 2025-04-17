import "./styles/App.css";
import { Toolbar } from "./Toolbar";
import { CodeEditor } from "./Editor";
import { FileProvider } from "./FileEventsHandler";
import { LiveShareProvider } from "./FileContentContext";
function App() {
  return (
    <LiveShareProvider>
      <FileProvider>
        <div className="app-container">
          <Toolbar />
          <CodeEditor />
        </div>
      </FileProvider>
    </LiveShareProvider>
  );
}

export default App;
